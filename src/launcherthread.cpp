#include "launcherthread.h"
#include "launcherexception.h"
#include <QDebug>
#include <QException>
#include <QFile>

void LauncherThread::run()
{
    try
    {
        emit progressChanged(0);
        emit statusChanged(QString("Waiting..."));

        LauncherData data = loadData();

        qDebug() << data.patcherSecret.toStdString().c_str();

        int version = m_remotePatcher->getVersion(data.patcherSecret);

        qDebug() << version;

        m_remotePatcher->download(data.patcherSecret, version);

        emit progressChanged(100);
    }
    catch(QException &exception)
    {
        qCritical(exception.what());
    }
    catch(...)
    {
        qFatal("Unhandled exception of unknown type.");
    }
}

LauncherThread::LauncherThread(const LauncherConfiguration &configuration, RemotePatcher * const remotePatcher) :
    m_remotePatcher(remotePatcher),
    m_configuration(configuration),
    m_isCancelled(false)
{
    m_remotePatcher->moveToThread(this);
}

void LauncherThread::cancel()
{
    m_isCancelled = true;
}

LauncherData LauncherThread::loadData()
{
    qDebug() << QString("Searching for data file - %1").arg(m_configuration.dataFileName).toStdString().c_str();
    if(!QFile::exists(m_configuration.dataFileName))
    {
        throw LauncherException("Missing data file.");
    }
    qDebug() << "Loading data file.";
    return LauncherData::loadFromFile(m_configuration.dataFileName);
}
