/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include <QtMath>

#include "launcherthread.h"
#include "launcherexception.h"
#include "launchercancelledexception.h"
#include "launcherlog.h"


void LauncherThread::run()
{
    try
    {
        logInfo("Starting launcher operation.");

        emit progressChanged(0);
        emit statusChanged(QString("Waiting..."));

        LauncherData data = LauncherData::loadFromFile(m_configuration.dataFileName);

        try
        {
            int version = m_remotePatcher->getVersion(data);

            logDebug("Current remote patcher version - %1", .arg(QString::number(version)));

            if (m_localPatcher->isInstalled())
            {
                logInfo("Found patcher installation.");
                if (version != m_localPatcher->getVersion())
                {
                    logInfo("Local version is different from remote version. Uninstalling local patcher so new version will be installed.");
                    m_localPatcher->uninstall();
                }
            }

            if (!m_localPatcher->isInstalled())
            {
                logInfo("Patcher is not installed. Downloading the newest version of patcher.");
                emit statusChanged(QString("Downloading..."));

                logDebug("Connecting downloadProgressChanged signal from remote patcher to slot from launcher thread.");
                connect(m_remotePatcher.get(), &RemotePatcher::downloadProgressChanged, this, &LauncherThread::setDownloadProgress);

                QString downloadedPath = m_remotePatcher->download(data, version);
                logInfo("Patcher has been downloaded to %1", .arg(downloadedPath));

                logDebug("Disconnecting downloadProgressChanged signal from remote patcher to slot from launcher thread.");
                disconnect(m_remotePatcher.get(), &RemotePatcher::downloadProgressChanged, this, &LauncherThread::setDownloadProgress);

                emit progressChanged(downloadedProgressValue);

                emit statusChanged(QString("Installing..."));

                m_localPatcher->install(downloadedPath, version);
                logInfo("Patcher has been installed.");

                emit progressChanged(100);
            }
        }
        catch (QException& exception)
        {
            logWarning(exception.what());

            logInfo("Fallback - trying to use previous patcher installation.");

            if (!m_localPatcher->isInstalled())
            {
                logInfo("Fallback success - found previous patcher installation.");
            }
            else
            {
                logCritical("Fallback failed - no previous patcher installation.");
                throw;
            }
        }

        emit statusChanged(QString("Starting..."));

        m_localPatcher->start(data);
    }
    catch (LauncherCancelledException&)
    {
        logWarning("Launcher has been canceled.");
    }
    catch (QException& exception)
    {
        logCritical(exception.what());
    }
    catch (...)
    {
        logCritical("Unhandled exception of unknown type.");
    }
}

LauncherThread::LauncherThread(const LauncherConfiguration& t_configuration,
                               std::shared_ptr<RemotePatcher> t_remotePatcher,
                               std::shared_ptr<LocalPatcher> t_localPatcher) :
    m_remotePatcher(t_remotePatcher),
    m_localPatcher(t_localPatcher),
    m_configuration(t_configuration),
    m_isCancelled(false)
{
    logDebug("Moving m_remotePatcher and m_localPatcher to launcher thread.");
    m_remotePatcher->moveToThread(this);
    m_localPatcher->moveToThread(this);
}

void LauncherThread::cancel()
{
    logInfo("Cancelling launcher thread");

    m_isCancelled = true;

    /* We don't know from which thread this method is called so we need to make sure that both RemotePatcher::cancel
       and LocalPatcher::cancel are called from their threads.
    */
    QMetaObject::invokeMethod(m_remotePatcher.get(), "cancel");
    QMetaObject::invokeMethod(m_localPatcher.get(), "cancel");
}

void LauncherThread::setDownloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes)
{
    long long kilobytesDownloaded = t_bytesDownloaded / 1024;
    long long totalKilobytes = t_totalBytes / 1024;

    emit statusChanged(QString("Downloading %1 / %2 KB").arg(QString::number(kilobytesDownloaded), QString::number(totalKilobytes)));
    emit progressChanged(qCeil((qreal(t_bytesDownloaded) / t_totalBytes) * downloadedProgressValue));
}
