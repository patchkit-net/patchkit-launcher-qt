/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcherworker.h"

#include <QtMath>
#include <QSettings>

#include "cancellation/cancellationtoken.h"
#include "data/launchersettings.h"
#include "logger.h"
#include "locations.h"
#include "customexceptions.h"
#include "config.h"
#include "lockfile.h"
#include "utilities.h"

void LauncherWorker::run()
{
    // Check permissions
    if (!Utilities::isCurrentDirectoryWritable())
    {
        Utilities::tryRestartWithHigherPermissions();
        m_result = Result::RESTART;
        return;
    }

    // Initialize
    Data data = resolveData();

    // Initialize components
    QNetworkAccessManager nam;
    Api api(nam);
    LocalPatcherData localData;

    LauncherSettings settings(data.applicationSecret());

    try // TODO: Figure out why is this here
    {
        QString newPatcherSecret = api.getPatcherSecret(data.applicationSecret(), m_cancellationTokenSource);

        settings.savePatcherSecret(newPatcherSecret);

        data = Data::overwritePatcherSecret(data, newPatcherSecret);
    }
    catch (CancellationToken::CancelledException)
    {
        throw;
    }
    catch (...)
    {
        if (!settings.patcherSecret().isEmpty())
        {
            data = Data::overwritePatcherSecret(data, settings.patcherSecret());
        }
    }

    LockFile lockFile;
    lockFile.lock();
    while (true)
    {
        try
        {
            // Update
        }
        catch(Api::ApiConnectionError& e)
        {
            if (localData.isInstalled())
            {
//                m_launcherInterface.shouldRetry()
            }
        }
    }
}

LauncherWorker::LauncherWorker(ILauncherInterface& launcherInterface, QObject* parent)
    : QThread(parent)
    , m_result(NONE)
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

LauncherWorker::Result LauncherWorker::result() const
{
    return m_result;
}

void LauncherWorker::setDownloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes)
{
    long long kilobytesDownloaded = t_bytesDownloaded / 1024;
    long long totalKilobytes = t_totalBytes / 1024;

    emit statusChanged(QString("Downloading %1 / %2 KB").arg(QString::number(kilobytesDownloaded), QString::number(totalKilobytes)));
    emit progressChanged(qCeil((qreal(t_bytesDownloaded) / t_totalBytes) * 100.0));
}
