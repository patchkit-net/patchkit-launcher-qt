/*
* Copyright (C) Upsoft 2017
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
#include "lockfile.h"
#include "utilities.h"

void LauncherWorker::run()
{
    try
    {
        Data data = resolveData();

        runWithData(data);

        m_result = SUCCESS;
        qInfo("Launcher has succeeded.");
    }
    catch(Api::ServerConnectionError& e)
    {
        m_result = CONNECTION_ERROR;
        qCritical() << e.what();
    }
    catch (LockFile::LockException&)
    {
        m_result = LOCKED;
        qCritical("Lock file detected.");
    }
    catch (CancellationToken::CancelledException&)
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

LauncherWorker::LauncherWorker(QObject* parent)
    : QThread(parent)
    , m_api(&m_networkAccessManager)
    , m_remotePatcher(m_api, &m_networkAccessManager)
    , m_result(NONE)
{
    m_api.moveToThread(this);
    m_networkAccessManager.moveToThread(this);
    m_remotePatcher.moveToThread(this);
    m_localPatcher.moveToThread(this);
}

void LauncherWorker::cancel()
{
    qInfo("Cancelling launcher thread.");

    m_cancellationTokenSource.cancel();
}

Data LauncherWorker::resolveData() const
{
    qInfo("Resolving data.");
    if (Data::canLoadFromConfig())
    {
        try
        {
            qInfo("Loaded inline data.");
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
        qInfo("Loaded data from resource.");
        return Data::loadFromResources(Locations::getInstance().applicationFilePath(),
                                            Config::dataResourceId,
                                            Config::dataResourceTypeId);
    }
    catch(std::exception& e)
    {
        qWarning(e.what());
    }

#endif

    qInfo("Loaded data from file.");
    return Data::loadFromFile(Locations::getInstance().dataFilePath());
}

LauncherWorker::Result LauncherWorker::result() const
{
    return m_result;
}

void LauncherWorker::stop()
{
    m_cancellationTokenSource.cancel();
}

void LauncherWorker::setDownloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes)
{
    long long kilobytesDownloaded = t_bytesDownloaded / 1024;
    long long totalKilobytes = t_totalBytes / 1024;

    emit statusChanged(QString("Downloading %1 / %2 KB").arg(kilobytesDownloaded).arg(totalKilobytes));
    emit progressChanged(qCeil((qreal(t_bytesDownloaded) / t_totalBytes) * 100.0));
}

void LauncherWorker::runWithData(const Data& t_data)
{
    Locations::getInstance().initializeWithData(t_data);
    qInfo() << "Current directory set to - " << Locations::getInstance().currentDirPath();

    if (!Utilities::isCurrentDirectoryWritable())
    {
        Utilities::tryRestartWithHigherPermissions();
    }

    LockFile lockFile;

    try
    {
        emit progressChanged(0);
        emit statusChanged("Initializing...");

        qInfo("Starting launcher.");
        Data dataWithPatcherSecret = setupPatcherSecret(t_data);

        while (true)
        {
            try
            {
                qInfo("Trying to update Patcher");
                updatePatcher(t_data);
            }
            catch (Api::ServerConnectionError&)
            {
                if (!ask("Connection issues", "Launcher is experiencing connection issues, would you like to continue?"))
                {
                    throw;
                }
            }
        }
    }
    catch (CancellationToken::CancelledException&)
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

    lockFile.cede();
    try
    {
        startPatcher(t_data);
    }
    catch(...)
    {
        lockFile.clear();
        throw;
    }
}

Data LauncherWorker::setupPatcherSecret(const Data& t_data)
{
    auto maybePatcherSecret = tryToFetchPatcherSecret(t_data);
    QSettings settings("UpSoft", t_data.encodedApplicationSecret().append("launcher-"));
    const QString PATCHER_SECRET_KEY = "patcher_secret";

    if (maybePatcherSecret.is)
    {
        qInfo("Setting up with downloaded patcher secret.");
        settings.setValue(PATCHER_SECRET_KEY, maybePatcherSecret.value);
        return Data(maybePatcherSecret.value, t_data.encodedApplicationSecret());
    }
    else if (settings.contains(PATCHER_SECRET_KEY))
    {
        qInfo("Setting up with patcher secret from settings.");
        return Data(settings.value(PATCHER_SECRET_KEY).toString(), t_data.encodedApplicationSecret());
    }

    qInfo("Setting up with patcher secret from data.");
    return t_data;
}

Utilities::Maybe<QString> LauncherWorker::tryToFetchPatcherSecret(const Data& t_data)
{
    try
    {
        QString patcherSecret = m_remotePatcher.getPatcherSecret(t_data.applicationSecret(), m_cancellationTokenSource);
        return Utilities::Maybe<QString>(patcherSecret != QString(), patcherSecret);
    }
    catch (CancellationToken::CancelledException&)
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
        return Utilities::Maybe<QString>(false, QString());
    }
    catch (...)
    {
        qWarning("Unknown exception while fetching patcher secret.");
        return Utilities::Maybe<QString>(false, QString());
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
        qInfo("The newest patcher is not installed. Downloading the newest version of patcher.");

        emit statusChanged("Downloading...");

        qDebug("Connecting downloadProgressChanged signal from remote patcher to slot from launcher thread.");
        connect(&m_remotePatcher, &RemotePatcherData::downloadProgressChanged, this, &LauncherWorker::setDownloadProgress);

        QString downloadPath = Locations::getInstance().patcherDownloadPath();

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

bool LauncherWorker::ask(const QString& t_title, const QString& t_message)
{
    bool answer;;
    QEventLoop loop;

    connect(this, &LauncherWorker::queryAnswer, [&loop, &answer] (bool ans) {
        loop.quit();
        answer = ans;
    });

    emit query(t_title, t_message);
    loop.exec();

    return answer;
}
