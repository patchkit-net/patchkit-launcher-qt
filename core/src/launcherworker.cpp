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
        catch (std::exception& e)
        {
            qCritical() << e.what();
            if (!m_launcherInterface.shouldRetry(e.what()))
            {
                return;
            }
        }
    }
}

bool LauncherWorker::runInternal()
{
    // Check permissions
    if (!Utilities::isCurrentDirectoryWritable())
    {
        throw InsufficientPermissions("Launcher needs the current directory to be writable");
    }

    qInfo() << "Initializing";
    // Initialize
    Data data = resolveData();

    // Initialize components
    QNetworkAccessManager nam;
    Api api(nam);

    // TODO: Initialize the locations as a non-static structure

    // Setup the patcher secret
    // NOTE: Why?
    data = setupPatcherSecret(data, api, m_cancellationTokenSource);

    // Lock instance
    LockFile lockFile;
    lockFile.lock();

    LocalPatcherData localData;

    qInfo() << "Updating the patcher";
    bool hasUpdated = tryUpdate(data, api, nam, m_cancellationTokenSource);

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
        else
        {
        }
    }

    return false;
}

LauncherWorker::LauncherWorker(ILauncherInterface& launcherInterface, QObject* parent)
    : QThread(parent)
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
        return Data::loadFromResources(Locations::getInstance().applicationFilePath(),
                                            Config::dataResourceId,
                                            Config::dataResourceTypeId);
    }
    catch(std::exception& e)
    {
        qWarning() << e.what();
    }

#endif

    qInfo("Loading data from file.");
    return Data::loadFromFile(Locations::getInstance().dataFilePath());
}

void LauncherWorker::update(const Data& data, const Api& api, QNetworkAccessManager& nam, CancellationToken cancellationToken)
{
    LocalPatcherData localData;

    int latestAppVersion = api.getLatestAppVersion(data.patcherSecret(), cancellationToken);

    if (localData.isInstalledSpecific(latestAppVersion, data))
    {
        qInfo() << "Latest version is already installed";
        return;
    }

    ContentSummary contentSummary = api.getContentSummary(data.patcherSecret(), latestAppVersion, cancellationToken);

    QBuffer downloadData;
    downloadData.open(QIODevice::ReadWrite);
    downloading::chunked::Downloader downloader(
                data.patcherSecret(), latestAppVersion, contentSummary, downloadData);

    while (true)
    {
        if (downloader.downloadChunked(api, nam, cancellationToken))
        {
            break;
        }
    }

    LocalPatcherData local;
    local.install(downloadData, data, latestAppVersion);
}

bool LauncherWorker::tryUpdate(const Data& data, const Api& api, QNetworkAccessManager& nam, CancellationToken cancellationToken)
{
    int invalidChunkCount = 0;

    while (true)
    {
        try
        {
            update(data, api, nam, cancellationToken);
            return true;
        }
        catch (downloading::chunked::Downloader::InvalidTarget&)
        {
            qWarning() << "Target contains invalid data and will be cleaned up.";
            QFile(Locations::getInstance().patcherDownloadPath()).remove();
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
    catch (CancellationToken::CancelledException)
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

void LauncherWorker::setDownloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes)
{
    long long kilobytesDownloaded = t_bytesDownloaded / 1024;
    long long totalKilobytes = t_totalBytes / 1024;

    emit statusChanged(QString("Downloading %1 / %2 KB").arg(QString::number(kilobytesDownloaded), QString::number(totalKilobytes)));
    emit progressChanged(qCeil((qreal(t_bytesDownloaded) / t_totalBytes) * 100.0));
}
