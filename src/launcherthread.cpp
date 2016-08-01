#include "launcherthread.h"
#include "launcherexception.h"
#include "launchercancelledexception.h"
#include <QDebug>
#include <QException>
#include <QFile>
#include <QtMath>
#include <QString>

void LauncherThread::run()
{
    try
    {
        emit progressChanged(0);
        emit statusChanged(QString("Waiting..."));

        LauncherData data = loadData();

        qDebug() << data.patcherSecret.toStdString().c_str();

        int version = m_remotePatcher->getVersion(data.patcherSecret);

        emit statusChanged(QString("Downloading..."));

        m_remotePatcher->download(data.patcherSecret, version);

        emit progressChanged(90);

        emit statusChanged(QString("Installing..."));

        emit progressChanged(100);
    }
    catch(LauncherCancelledException&)
    {
        qWarning("Launcher has been canceled.");
    }
    catch(QException& exception)
    {
        qCritical(exception.what());
    }
    catch(...)
    {
        qFatal("Unhandled exception of unknown type.");
    }
}

LauncherThread::LauncherThread(const LauncherConfiguration& t_configuration, RemotePatcher * const t_remotePatcher) :
    m_remotePatcher(t_remotePatcher),
    m_configuration(t_configuration),
    m_isCancelled(false)
{
    m_remotePatcher->moveToThread(this);

    connect(m_remotePatcher, &RemotePatcher::downloadProgress, this, &LauncherThread::downloadProgress);
    connect(this, &LauncherThread::cancelled, m_remotePatcher, &RemotePatcher::cancel);
}

void LauncherThread::cancel()
{
    m_isCancelled = true;

    emit cancelled();
}

void LauncherThread::downloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes)
{
    long long kilobytesDownloaded = t_bytesDownloaded / 1024;
    long long totalKilobytes = t_totalBytes / 1024;

    emit statusChanged(QString("Downloading %1 / %2 KB").arg(QString::number(kilobytesDownloaded), QString::number(totalKilobytes)));
    emit progressChanged(qCeil((qreal(t_bytesDownloaded)/t_totalBytes) * 0.9 * 100));
}

LauncherData LauncherThread::loadData() const
{
    qDebug() << QString("Searching for data file - %1").arg(m_configuration.dataFileName).toStdString().c_str();
    if(!QFile::exists(m_configuration.dataFileName))
    {
        throw LauncherException("Missing data file.");
    }
    qDebug() << "Loading data file.";
    return LauncherData::loadFromFile(m_configuration.dataFileName);
}
