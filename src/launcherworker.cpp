/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcherworker.h"

#include <QtMath>

#include "logger.h"
#include "locations.h"

void LauncherWorker::run()
{
    try
    {
#ifdef Q_OS_WIN
        try
        {
            runWithDataFromResource();
            return;
        }
        catch (CancelledException&)
        {
            throw;
        }
        catch (QException& exception)
        {
            logWarning(exception.what());
        }
        catch (...)
        {
            logWarning("Unknown exception.");
        }

        logWarning("Running with data from resources has failed. Trying to run with data from file located in current directory.");
#endif

        runWithDataFromFile();
        m_noError = true;
    }
    catch (CancelledException&)
    {
        m_noError = true;
        logInfo("Launcher has been canceled.");
    }
    catch (QException& exception)
    {
        m_noError = false;
        logCritical(exception.what());
    }
    catch (...)
    {
        m_noError = false;
        logCritical("Unknown exception.");
    }
}

LauncherWorker::LauncherWorker() :
    m_cancellationTokenSource(new CancellationTokenSource()),
    m_noError(false)
{
    m_remotePatcher.moveToThread(this);
    m_localPatcher.moveToThread(this);
}

void LauncherWorker::cancel()
{
    logInfo("Cancelling launcher thread");

    m_cancellationTokenSource->cancel();
}

bool LauncherWorker::noError() const
{
    return m_noError;
}

void LauncherWorker::setDownloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes)
{
    long long kilobytesDownloaded = t_bytesDownloaded / 1024;
    long long totalKilobytes = t_totalBytes / 1024;

    emit statusChanged(QString("Downloading %1 / %2 KB").arg(QString::number(kilobytesDownloaded), QString::number(totalKilobytes)));
    emit progressChanged(qCeil(qreal(t_bytesDownloaded) / t_totalBytes));
}

#ifdef Q_OS_WIN

void LauncherWorker::runWithDataFromResource()
{
    logInfo("Starting launcher with data from resource.");

    Data data = Data::loadFromResources(Locations::applicationFilePath(),
                                        Config::dataResourceId,
                                        Config::dataResourceTypeId);

    runWithData(data);
}

#endif

void LauncherWorker::runWithDataFromFile()
{
    logInfo("Starting launcher with data from file.");

    Data data = Data::loadFromFile(Locations::dataFilePath());

    runWithData(data);
}

void LauncherWorker::runWithData(Data& t_data)
{
    try
    {
        logInfo("Starting launcher.");

        QSettings settings("UpSoft", t_data.applicationSecret().remove(0, 2).append("launcher-"));

        if(tryToFetchPatcherSecret(t_data))
        {
            settings.setValue("patcher_sercet", t_data.overwritePatcherSecret);
        }
        else if (settings.contains("patcher_secret"))
        {
            t_data.overwritePatcherSecret = settings.value("patcher_secret").toString();
        }

        setupCurrentDirectory(t_data);

        logInfo("Current directory set to - %1", .arg(Locations::currentDirPath()));

        updatePatcher(t_data);
    }
    catch (CancelledException&)
    {
        throw;
    }
    catch (QException& exception)
    {
        if (m_localPatcher.isInstalled())
        {
            logWarning(exception.what());
            logWarning("Updating patcher failed but previous patcher is still available.");
        }
        else
        {
            throw;
        }
    }
    catch (...)
    {
        if (m_localPatcher.isInstalled())
        {
            logWarning("Unknown exception.");
            logWarning("Updating patcher failed but previous patcher is still available.");
        }
        else
        {
            throw;
        }
    }

    startPatcher(t_data);
}

bool LauncherWorker::tryToFetchPatcherSecret(Data& t_data)
{
    try
    {
        t_data.overwritePatcherSecret = m_remotePatcher.getPatcherSecret(t_data, m_cancellationTokenSource);
        return true;
    }
    catch(CancelledException&)
    {
        throw;
    }
    catch (QException& exception)
    {
        logWarning(exception.what());
        return false;
    }
    catch(...)
    {
        logWarning("Unknown exception while fetching patcher secret.");
        return false;
    }
}

void LauncherWorker::setupCurrentDirectory(const Data& t_data) const
{
    logInfo("Setting current directory.");

#if defined(Q_OS_OSX)
    QDir resourcesDir(Locations::applicationDirPath());
    resourcesDir.cdUp();

	if(!resourcesDir.exists("Resources"))
	{
		resourcesDir.mkdir("Resources");
	}
    resourcesDir.cd("Resources");

    Locations::setCurrentDirPath(resourcesDir.absolutePath());
#elif defined(Q_OS_WIN)

    QString appDataDirName = t_data.applicationSecret().mid(0, 16);

    // TODO: HACK - QStandardPaths is using <APPNAME> as part of the QStandardPaths::AppDataLocation path. Because default name of application is "Launcher" we need to temporarly change it to the unique identifier of application.

    QString previousApplicationName = QCoreApplication::applicationName();
    QCoreApplication::setApplicationName(appDataDirName);

    QDir appDataDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

    QCoreApplication::setApplicationName(previousApplicationName);

    if (!appDataDir.exists())
    {
        appDataDir.mkpath(".");
    }

    Locations::setCurrentDirPath(appDataDir.absolutePath());
#else
    Locations::setCurrentDirPath(Locations::applicationDirPath());
#endif
}

void LauncherWorker::updatePatcher(const Data& t_data)
{
    logInfo("Updating patcher.");

    int version = m_remotePatcher.getVersion(t_data, m_cancellationTokenSource);
    logDebug("Current remote patcher version - %1", .arg(QString::number(version)));

    emit progressChanged(0);
    emit statusChanged(QString("Waiting..."));

    if (!m_localPatcher.isInstalledSpecific(version, t_data))
    {
        logInfo("The newest patcher is not installed. Downloading the newest version of patcher.");

        emit statusChanged(QString("Downloading..."));

        logDebug("Connecting downloadProgressChanged signal from remote patcher to slot from launcher thread.");
        connect(&m_remotePatcher, &RemotePatcherData::downloadProgressChanged, this, &LauncherWorker::setDownloadProgress);

        QString downloadedPath = m_remotePatcher.download(t_data, version, m_cancellationTokenSource);
        logInfo("Patcher has been downloaded to %1", .arg(downloadedPath));

        logDebug("Disconnecting downloadProgressChanged signal from remote patcher to slot from launcher thread.");
        disconnect(&m_remotePatcher, &RemotePatcherData::downloadProgressChanged, this, &LauncherWorker::setDownloadProgress);

        emit progressChanged(1.0f);
        emit statusChanged(QString("Installing..."));

        m_localPatcher.install(downloadedPath, t_data, version);
        logInfo("Patcher has been installed.");

        emit progressChanged(100);
    }
}

void LauncherWorker::startPatcher(const Data& t_data)
{
    logInfo("Starting patcher.");

    emit statusChanged(QString("Starting..."));

    m_localPatcher.start(t_data);
}
