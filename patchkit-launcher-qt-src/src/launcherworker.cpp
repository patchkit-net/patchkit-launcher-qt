/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcherworker.h"

#include <QtMath>
#include <QMessageBox>

#include "logger.h"
#include "locations.h"
#include "customexceptions.h"
#include "downloader.h"
#include "config.h"

#if defined(Q_OS_WIN)
#include <Windows.h>
#endif

void LauncherWorker::run()
{
    try
    {
        if (Data::canLoadFromConfig())
        {
            qInfo("Detected inlined data.");
            runWithInlineData();

            m_result = SUCCESS;
            qInfo("Launcher has succeeded.");
            return;
        }

#ifdef Q_OS_WIN
        try
        {
            runWithDataFromResource();
            return;
        }
        catch (std::exception& exception)
        {
            qWarning(exception.what());
        }

        qWarning("Running with data from resources has failed. Trying to run with data from file located in current directory.");
#endif

        runWithDataFromFile();

        m_result = SUCCESS;
        qInfo("Launcher has succeeded.");
    }
    catch (CancelledException&)
    {
        m_result = CANCELLED;
        qInfo("Launcher has been canceled.");
    }
    catch (FatalException& exception)
    {
        m_result = FATAL_ERROR;
        qCritical() << exception.what();
    }
    catch (std::exception& exception)
    {
        m_result = FAILED;
        qWarning() << exception.what();
    }
    catch (...)
    {
        m_result = FATAL_ERROR;
        qCritical("Unknown exception.");
    }
}

LauncherWorker::LauncherWorker(LauncherState& t_launcherState, QObject* parent)
    : QThread(parent)
    , m_cancellationTokenSource(new CancellationTokenSource())
    , m_launcherState(t_launcherState)
    , m_api(&m_networkAccessManager, CancellationToken(m_cancellationTokenSource), m_launcherState)
    , m_remotePatcher(m_launcherState, m_api, &m_networkAccessManager)
    , m_result(NONE)
    , m_shouldUpdate(true)
{
    m_api.moveToThread(this);
    m_networkAccessManager.moveToThread(this);
    m_remotePatcher.moveToThread(this);
    m_localPatcher.moveToThread(this);
}

void LauncherWorker::cancel()
{
    qInfo("Cancelling launcher thread.");

    m_cancellationTokenSource->cancel();
}

bool LauncherWorker::isLocalPatcherInstalled() const
{
    return m_localPatcher.isInstalled();
}

LauncherWorker::Result LauncherWorker::result() const
{
    return m_result;
}

void LauncherWorker::stopUpdate()
{
    m_shouldUpdate = false;
}

void LauncherWorker::setDownloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes)
{
    long long kilobytesDownloaded = t_bytesDownloaded / 1024;
    long long totalKilobytes = t_totalBytes / 1024;

    emit statusChanged(QString("Downloading %1 / %2 KB").arg(QString::number(kilobytesDownloaded), QString::number(totalKilobytes)));
    emit progressChanged(qCeil((qreal(t_bytesDownloaded) / t_totalBytes) * 100.0));
}

#ifdef Q_OS_WIN

void LauncherWorker::runWithDataFromResource()
{
    qInfo("Starting launcher with data from resource.");

    Data data = Data::loadFromResources(Locations::getInstance().applicationFilePath(),
                                        Config::dataResourceId,
                                        Config::dataResourceTypeId);

    runWithData(data);
}

#endif

void LauncherWorker::runWithDataFromFile()
{
    qInfo("Starting launcher with data from file.");

    Data data = Data::loadFromFile(Locations::getInstance().dataFilePath());

    runWithData(data);
}

void LauncherWorker::runWithInlineData()
{
    qInfo("Starting launcher with inlined data.");

    Data data = Data::loadFromConfig();

    runWithData(data);
}

void LauncherWorker::runWithData(Data& t_data)
{
    try
    {
        emit progressChanged(0);
        emit statusChanged("Waiting...");

        qInfo("Starting launcher.");

        qInfo("Trying to obtain location information...");
        if (m_api.geolocate())
        {
            qInfo() << "Operation successful, country code resolved to: " << m_api.getCountryCode();
        }
        else
        {
            qWarning("Couldn't obtain the country code.");
        }

        setupPatcherSecret(t_data);

        if (!m_shouldUpdate)
        {
            throw std::runtime_error("Offline mode was requested.");
        }

        Locations::getInstance().initializeWithData(t_data);

        qInfo() << "Current directory set to - " << Locations::getInstance().currentDirPath();

        updatePatcher(t_data);
    }
    catch (CancelledException&)
    {
        throw;
    }
    catch (FatalException&)
    {
        throw;
    }
    catch (std::exception& exception)
    {
        if (m_localPatcher.isInstalled())
        {
            qWarning() << exception.what();
            qWarning("Updating patcher failed but previous patcher is still available.");
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
            qWarning("Unknown exception.");
            qWarning("Updating patcher failed but previous patcher is still available.");
        }
        else
        {
            throw;
        }
    }

    qDebug("Yay stariting the patcher.");
    // startPatcher(t_data);
}

void LauncherWorker::setupPatcherSecret(Data& t_data)
{
    QSettings settings("UpSoft", t_data.applicationSecret().append("launcher-"));

    if (tryToFetchPatcherSecret(t_data))
    {
        settings.setValue("patcher_secret", t_data.overwritePatcherSecret);
    }
    else if (settings.contains("patcher_secret"))
    {
        t_data.overwritePatcherSecret = settings.value("patcher_secret").toString();
    }
}

bool LauncherWorker::tryToFetchPatcherSecret(Data& t_data)
{
    try
    {
        t_data.overwritePatcherSecret = m_remotePatcher.getPatcherSecret(t_data, m_cancellationTokenSource);
        return true;
    }
    catch (CancelledException&)
    {
        throw;
    }
    catch (FatalException&)
    {
        throw;
    }
    catch (std::exception& exception)
    {
        qWarning() << exception.what();
        return false;
    }
    catch (...)
    {
        qWarning("Unknown exception while fetching patcher secret.");
        return false;
    }
}

void LauncherWorker::updatePatcher(const Data& t_data)
{
    qInfo("Updating patcher.");

    int version = -1;
    try
    {
        version = m_remotePatcher.getVersion(t_data, m_cancellationTokenSource);
    }
    catch(...)
    {
        throw;
    }

    qDebug("Current remote patcher version - %d", version);

    if (!m_localPatcher.isInstalledSpecific(version, t_data))
    {
        qInfo("Checking whether current directory is writable.");

        checkIfCurrentDirectoryIsWritable();

        qInfo("The newest patcher is not installed. Downloading the newest version of patcher.");

        emit statusChanged("Downloading...");

        qDebug("Connecting downloadProgressChanged signal from remote patcher to slot from launcher thread.");
        connect(&m_remotePatcher, &RemotePatcherData::downloadProgressChanged, this, &LauncherWorker::setDownloadProgress);

        QString downloadPath = QDir::cleanPath(Locations::getInstance().applicationDirPath() + "/patcher.zip");

        QFile file(downloadPath);

        m_remotePatcher.download(file, t_data, version, m_cancellationTokenSource);
        qInfo() << "Patcher has been downloaded to " << downloadPath;

        qDebug("Disconnecting downloadProgressChanged signal from remote patcher to slot from launcher thread.");
        disconnect(&m_remotePatcher, &RemotePatcherData::downloadProgressChanged, this, &LauncherWorker::setDownloadProgress);

        emit progressChanged(100);
        emit statusChanged("Installing...");

        m_localPatcher.install(downloadPath, t_data, version);

        QFile::remove(downloadPath);
        qInfo("Patcher has been installed.");
    }
}

void LauncherWorker::startPatcher(const Data& t_data)
{
    qInfo("Starting patcher.");

    emit statusChanged("Starting...");

    m_localPatcher.start(t_data);
}

void LauncherWorker::checkIfCurrentDirectoryIsWritable()
{
    if (!Locations::getInstance().isCurrentDirWritable())
    {
#if defined(Q_OS_WIN)
        ShellExecute(nullptr, L"runas", Locations::getInstance().applicationFilePath().toStdWString().c_str(), nullptr, nullptr, SW_SHOWNORMAL);

        throw CancelledException();
#else
        throw FatalException("Current application directory isn't writable.");
#endif
    }
}
