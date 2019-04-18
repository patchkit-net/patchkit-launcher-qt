/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcherworker.h"

#include <QtMath>
#include <QSettings>
#include <QFile>

#include "cancellation/cancellationtoken.h"
#include "data/launchersettings.h"
#include "remote/downloading/chunked/downloader.h"
#include "remote/downloading/progressdevice.h"
#include "logger.h"
#include "locations.h"
#include "customexceptions.h"
#include "config.h"
#include "lockfile.h"
#include "utilities.h"

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
        catch (Api::ApiConnectionError& e)
        {
            qCritical() << e.what();
            if (retryOrGoOffline("Failed to connect to the patchkit API."))
            {
                return;
            }
        }
        catch (InvalidFormatException& e)
        {
            qCritical() << e.what();
            if (retryOrGoOffline("Launcher failed to parse an API response."))
            {
                return;
            }
        }
        catch (std::exception& e)
        {
            qCritical() << e.what();
            if (retryOrGoOffline("An unknown error has occured."))
            {
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

    // Setup the patcher secret
    // NOTE: Why?
    data = setupPatcherSecret(data, api, m_cancellationTokenSource);
    m_runningData.reset(new Data(data));

    // Locations
    Locations locations(data);

    // Check permissions
    if (!Utilities::isDirectoryWritable(locations.currentDirPath()))
    {
        throw InsufficientPermissions("Launcher needs the current directory to be writable");
    }

    qInfo() << "Initialzing logger";
    Logger::initialize();

    // Lock instance
    LockFile lockFile;
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

bool LauncherWorker::retryOrGoOffline(const QString& reason)
{
    qInfo("Asking the user to either retry or go into offline mode");
    if (m_runningData)
    {
        Locations locations(*m_runningData);
        LocalPatcherData localData(locations);

        if (localData.isInstalled())
        {
            qInfo("Local patcher version is installed, asking if should go offline");
            auto ans = m_launcherInterface.shoulStartInOfflineMode();
            if (ans == ILauncherInterface::OfflineModeAnswer::YES)
            {
                qInfo("The user decided to go into offline mode");
                localData.start(*m_runningData, data::NetworkStatus::Offline);
                return true;
            }
            else if (ans == ILauncherInterface::OfflineModeAnswer::NO)
            {
                qInfo("The user decided not to into offline mode, launcher will retry");
                return false;
            }
            else
            {
                qInfo("The user decided to cancel");
                throw CancellationToken::CancelledException();
            }
        }
    }

    qInfo("Asking the user if Launcher should retry.");
    if (m_launcherInterface.shouldRetry(reason))
    {
        qInfo("The user decided to retry");
        return false;
    }
    else
    {
        return true;
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
        return Data::loadFromResources(Locations::applicationFilePath(),
                                            Config::dataResourceId,
                                            Config::dataResourceTypeId);
    }
    catch(std::exception& e)
    {
        qWarning() << e.what();
    }

#endif

    qInfo("Loading data from file.");
    return Data::loadFromFile(Locations::dataFilePath());
}

void LauncherWorker::update(
        const Locations& locations, const Data& data,
        const Api& api, QNetworkAccessManager& nam,
        CancellationToken cancellationToken)
{
    LocalPatcherData localData(locations);

    int latestAppVersion = api.getLatestAppVersion(data.patcherSecret(), cancellationToken);

    if (localData.isInstalledSpecific(latestAppVersion, data))
    {
        qInfo() << "Latest version is already installed";
        return;
    }

    ContentSummary contentSummary = api.getContentSummary(data.patcherSecret(), latestAppVersion, cancellationToken);


    QBuffer downloadData;
    downloadData.open(QIODevice::WriteOnly);

    ProgressDevice progressDevice(downloadData, contentSummary.getCompressedSize());
    progressDevice.open(QIODevice::WriteOnly);

    connect(&progressDevice, &ProgressDevice::onProgress, this, &LauncherWorker::setDownloadProgress);
    auto downloader = downloading::chunked::Downloader(
                data.patcherSecret(), latestAppVersion, contentSummary, progressDevice);

    if (!downloader.downloadChunked(api, nam, cancellationToken))
    {
        throw UpdateFailed("Failed to update");
    }

    emit progressChanged(100);

    downloadData.open(QIODevice::ReadOnly);

    emit statusChanged("Installing...");
    localData.install(downloadData, data, latestAppVersion);
}

bool LauncherWorker::tryUpdate(
        const Locations& locations,
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
