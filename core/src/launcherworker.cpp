/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcherworker.h"

#include <QtMath>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QProcessEnvironment>
#include <QGuiApplication>

#include "cancellation/cancellationtoken.h"
#include "data/launchersettings.h"
#include "remote/downloading/chunked/downloader.h"
#include "remote/downloading/progressdevice.h"
#include "logger.h"
#include "customexceptions.h"
#include "config.h"
#include "lockfile.h"
#include "utilities.h"
#include "locations/launcher.h"
#include "locations/installation.h"
#include "networktest.h"

void LauncherWorker::run()
{
    while(true)
    {
        try
        {
            if (runInternal())
            {
                qInfo() << "Launcher has finished successfully";
                return;
            }
            else
            {
                qWarning() << "Launcher has failed for unknown reasons but will retry";
            }
        }
        catch (InsufficientPermissions& e)
        {
            qWarning() << "Permissions error: " << e.what();
            Utilities::tryRestartWithHigherPermissions();
            return;
        }
        catch (CancellationToken::CancelledException&)
        {
            qInfo() << "Launcher has been cancelled";
            return;
        }
        catch (LockFile::LockException&)
        {
            qWarning() << "Another instance of launcher or patcher is running.";
            this->m_launcherInterface.displayErrorMessage("Another instance of launcher or patcher is running.");
            return;
        }
        catch (Api::ApiConnectionError& e)
        {
            qCritical() << e.what();
            switch (retryOrGoOffline("Failed to connect to the patchkit API."))
            {
                case Action::QUIT:
                    return;
                case Action::RETRY:
                    break;
                case Action::GO_OFFLINE:
                    tryStartOfflineOrDisplayError("Failed to start patcher in offline mode");
                    return;
            }
        }
        catch (InvalidFormatException& e)
        {
            qCritical() << e.what();
            switch (retryOrGoOffline("Launcher encountered an invalid format."))
            {
                case Action::QUIT:
                    return;
                case Action::RETRY:
                    break;
                case Action::GO_OFFLINE:
                    tryStartOfflineOrDisplayError("Failed to start patcher in offline mode");
                    return;
            }
        }
        catch (FatalException& e)
        {
            qCritical() << "A fatal exception has occured: " << e.what();
            m_launcherInterface.displayErrorMessage(e.what());
            return;
        }
        catch (std::exception& e)
        {
            qCritical() << e.what();
            switch (retryOrGoOffline("An unknown error has occured."))
            {
                case Action::QUIT:
                    return;
                case Action::RETRY:
                    break;
                case Action::GO_OFFLINE:
                    tryStartOfflineOrDisplayError("Failed to start patcher in offline mode");
                    return;
            }
        }
    }
}

bool LauncherWorker::runInternal()
{
    emit statusChanged("Initializing...");
    qInfo() << "Initializing";

    // Initialize
    Data data = resolveData();

    // Initialize components
    QNetworkAccessManager nam;
    Api api(nam);

    // Testing connectivity
    NetworkTest networkTest;
    if (!networkTest.isOnline(nam, m_cancellationTokenSource))
    {
        switch (retryOrGoOffline("Launcher cannot establish an internet connection"))
        {
            case Action::QUIT:
                return true;
            case Action::RETRY:
                return false;
            case Action::GO_OFFLINE:
                if (tryStartOffline())
                {
                    return true;
                }
                else
                {
                    if (m_launcherInterface.shouldRetry("Launcher failed to start patcher in offline mode"))
                    {
                        return false;
                    }
                    else
                    {
                        return true;
                    }
                }
        }
    }

    QString workingDir = locations::workingDirectory(data.applicationSecret());

    // Setup the patcher secret
    // NOTE: Why?
    data = setupPatcherSecret(data, api, m_cancellationTokenSource);
    m_runningData.reset(new Data(data));

    m_runningData.operator bool();

    locations::Installation installation;
    if (locations::Installation::canLoad(workingDir))
    {
        installation = locations::Installation::load(workingDir);
    }
    else
    {
        QString installationLocation = workingDir;
        if (Config::isSelectableInstallationLocationEnabled())
        {
            bool shouldCancel;
            m_launcherInterface.selectInstallationLocation(installationLocation, shouldCancel);
            if (shouldCancel)
            {
                CancellationToken::cancelHere();
            }

        }

        installation = locations::Installation(
                    QDir(installationLocation).filePath(Config::patcherDirectoryName),
                    QDir(installationLocation).filePath(Config::applicationDirectoryName));
        installation.save(workingDir);
    }

    locations::Launcher locations = locations::Launcher::initalize(
                data.applicationSecret(), installation);

    // Check permissions
    if (!Utilities::isDirectoryWritable(locations.directory()))
    {
        throw InsufficientPermissions("Launcher needs the current directory to be writable");
    }

    qInfo() << "Initialzing logger";
    Logger::initialize(workingDir);

    // Lock instance
    LockFile lockFile(locations.lockFile());
    lockFile.lock();

    LocalPatcherData localData(locations);

    qInfo() << "Updating the patcher";
    bool hasUpdated = tryUpdate(locations, data, api, nam, m_cancellationTokenSource);

    if (hasUpdated)
    {
        localData.start(data, data::NetworkStatus::Online);
        return true;
    }
    else
    {
        if (localData.isInstalled())
        {
            qWarning() << "Updating failed but a version of patcher is installed...";

            auto ans = m_launcherInterface.shoulStartInOfflineMode();
            if (ans == ILauncherInterface::OfflineModeAnswer::YES)
            {
                qInfo() << "Starting the patcher in offline mode";
                localData.start(data, data::NetworkStatus::Offline);
                return true;
            }
            else if (ans == ILauncherInterface::OfflineModeAnswer::NO)
            {
                return false;
            }
            else
            {
                throw CancellationToken::CancelledException();
            }
        }
    }

    return false;
}

bool LauncherWorker::tryStartOffline()
{
    if (m_runningData)
    {
        try
        {
            auto locations = this->tryLoadLocations();
            if (locations)
            {
                LocalPatcherData localData(*locations);
                localData.start(*m_runningData, data::NetworkStatus::Offline);
                return true;
            }

            return false;
        }
        catch (CancellationToken::CancelledException)
        {
            throw;
        }
        catch (std::exception&)
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

void LauncherWorker::tryStartOfflineOrDisplayError(const QString& msg)
{
    if (!tryStartOffline())
    {
        m_launcherInterface.displayErrorMessage(msg);
    }
}

void LauncherWorker::trySetDisplayName(const remote::api::Api& api, const Data& data, CancellationToken cancellationToken)
{
    try
    {
        AppInfo appInfo = api.getAppInfo(data.applicationSecret(), cancellationToken);
        if (!appInfo.displayName.isEmpty())
        {
            auto app = dynamic_cast<QGuiApplication*>(QGuiApplication::instance());
            if (app)
            {
                app->setApplicationDisplayName(appInfo.displayName);
            }
        }
    }
    catch (remote::api::Api::ApiConnectionError& e)
    {
       qWarning() << "Failed to set the display name due to an api connection error: " << e.what();
    }
    catch (InvalidFormatException& e)
    {
        qWarning() << "Failed to set the display name due to an invalid format error: " << e.what();
    }
    catch (TimeoutException&)
    {
        qWarning() << "Failed to set the display name due to a timeout";
    }
}

LauncherWorker::Action LauncherWorker::retryOrGoOffline(const QString& reason)
{
    qInfo("Asking the user to either retry or go into offline mode");
    if (m_runningData)
    {
        auto locations = this->tryLoadLocations();
        if (locations)
        {
            LocalPatcherData localData(*locations);

            if (localData.isInstalled())
            {
                qInfo("Local patcher version is installed, asking if should go offline");
                auto ans = m_launcherInterface.shoulStartInOfflineMode();
                if (ans == ILauncherInterface::OfflineModeAnswer::YES)
                {
                    qInfo("The user decided to go into offline mode");
                    return Action::GO_OFFLINE;
                }
                else if (ans == ILauncherInterface::OfflineModeAnswer::NO)
                {
                    qInfo("The user decided not to into offline mode, launcher will retry");
                    return Action::RETRY;
                }
                else
                {
                    qInfo("The user decided to cancel");
                    return Action::QUIT;
                }
            }
        }
    }

    qInfo("Asking the user if Launcher should retry.");
    if (m_launcherInterface.shouldRetry(reason))
    {
        qInfo("The user decided to retry");
        return Action::RETRY;
    }
    else
    {
        return Action::QUIT;
    }
}

LauncherWorker::LauncherWorker(ILauncherInterface& launcherInterface, QObject* parent)
    : QThread(parent)
    , m_runningData(nullptr)
    , m_launcherInterface(launcherInterface)
{
}

void LauncherWorker::cancel()
{
    qInfo("Cancelling launcher thread.");
    m_cancellationTokenSource.cancel();
}

Data LauncherWorker::resolveData()
{
    qInfo("Resolving data.");
    if (Data::canLoadFromConfig())
    {
        try
        {
            qInfo("Loading inline data.");
            return Data::loadFromConfig();
        }
        catch(std::exception& e)
        {
            qWarning() << e.what();
        }
    }

#ifdef Q_OS_WIN
    try
    {
        qInfo("Loading data from resource.");
        return Data::loadFromResources(locations::launcherExecutable(),
                                            Config::dataResourceId,
                                            Config::dataResourceTypeId);
    }
    catch(std::exception& e)
    {
        qWarning() << e.what();
    }

#endif

    qInfo("Loading data from file.");
    return Data::loadFromFile(locations::dataFilePath());
}

void LauncherWorker::update(
        const locations::Launcher& locations, const Data& data,
        const Api& api, QNetworkAccessManager& nam,
        CancellationToken cancellationToken)
{
    QProcessEnvironment env;
    LocalPatcherData localData(locations);

    int latestPatcherVersion = api.getLatestAppVersion(data.patcherSecret(), cancellationToken);
    if (env.contains(Config::patcherVersionIdOverrideEnvVar))
    {
        QString envVersionId = env.value(Config::patcherVersionIdOverrideEnvVar, "");
        latestPatcherVersion = envVersionId.toInt();
    }

    if (localData.isInstalledSpecific(latestPatcherVersion, data))
    {
        qInfo() << "Latest version is already installed";
        return;
    }

    trySetDisplayName(api, data, cancellationToken);

    ContentSummary contentSummary = api.getContentSummary(data.patcherSecret(), latestPatcherVersion, cancellationToken);

    QBuffer downloadData;
    downloadData.open(QIODevice::WriteOnly);

    ProgressDevice progressDevice(downloadData, contentSummary.getCompressedSize());
    progressDevice.open(QIODevice::WriteOnly);

    connect(&progressDevice, &ProgressDevice::onProgress, this, &LauncherWorker::setDownloadProgress);
    auto downloader = downloading::chunked::Downloader(
                data.patcherSecret(), latestPatcherVersion, contentSummary, progressDevice);

    if (!downloader.downloadChunked(api, nam, cancellationToken))
    {
        throw UpdateFailed("Failed to update");
    }

    emit progressChanged(100);

    downloadData.open(QIODevice::ReadOnly);

    emit statusChanged("Installing...");
    localData.install(downloadData, data, latestPatcherVersion, cancellationToken);
}

bool LauncherWorker::tryUpdate(
        const locations::Launcher& locations,
        const Data& data, const Api& api,
        QNetworkAccessManager& nam,
        CancellationToken cancellationToken)
{
    int invalidChunkCount = 0;

    try
    {
        update(locations, data, api, nam, cancellationToken);
        return true;
    }
    catch (downloading::chunked::ChunkedBuffer::ChunkVerificationException&)
    {
        qWarning() << "An invalid chunk has been detected in the data stream " << invalidChunkCount << " times";
        if (invalidChunkCount < Config::maxInvalidChunksCount)
        {
            qCritical() << "Incident count over threshold - update has failed.";
            return false;
        }
        else
        {
            qWarning() << "Incrementing counter and retrying";
            invalidChunkCount++;
        }
    }

    return false;
}

std::unique_ptr<locations::Launcher> LauncherWorker::tryLoadLocations()
{
    auto null = std::unique_ptr<locations::Launcher>(nullptr);

    QString appSecret = m_runningData->applicationSecret();
    QString workingDir = locations::workingDirectory(m_runningData->applicationSecret());
    if (!locations::Installation::canLoad(workingDir))
    {
        return null;
    }
    try
    {
        auto installation = locations::Installation::load(workingDir);
        auto locations = locations::Launcher::initalize(appSecret, installation);
        return std::make_unique<locations::Launcher>(locations);
    }
    catch (InvalidFormatException&)
    {
        return null;
    }
    catch (IOError&)
    {
        return null;
    }
    catch (FileNotFound&)
    {
        return null;
    }
}

Data LauncherWorker::setupPatcherSecret(const Data& data, const Api& api, CancellationToken cancellationToken)
{
    LauncherSettings settings(data.applicationSecret());

    try
    {
        QString newPatcherSecret = api.getPatcherSecret(
                    data.applicationSecret(), cancellationToken);

        settings.savePatcherSecret(newPatcherSecret);

        return Data::overwritePatcherSecret(data, newPatcherSecret);
    }
    catch (CancellationToken::CancelledException&)
    {
        throw;
    }
    catch (...)
    {
        if (!settings.patcherSecret().isEmpty())
        {
            return Data::overwritePatcherSecret(data, settings.patcherSecret());
        }
    }

    return data;
}

void LauncherWorker::setDownloadProgress(long long t_bytesDownloaded, long long t_totalBytes)
{
    auto kilobytesDownloaded = t_bytesDownloaded / 1024;
    auto totalKilobytes = t_totalBytes / 1024;

    emit statusChanged(QString("Downloading %1 / %2 KB").arg(QString::number(kilobytesDownloaded), QString::number(totalKilobytes)));
    emit progressChanged(qCeil((qreal(t_bytesDownloaded) / t_totalBytes) * 100.0));
}
