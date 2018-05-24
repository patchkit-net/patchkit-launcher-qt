#include "lockfile.h"

#include <QDebug>
#include <QDir>
#include <QFile>

#include "config.h"
#include "locations.h"
#include "customexceptions.h"

LockFile::LockFile()
    : m_isLockFileLocal(false)
{
    QFile lockFile(Config::lockFileName);
    if (!lockFile.exists())
    {
        lockFile.open(QIODevice::ReadWrite);
        lockFile.close();
    }

    m_lockFile = boost::interprocess::file_lock(Config::lockFileName.toStdString().c_str());
}

LockFile::~LockFile()
{
    if (m_isLockFileLocal)
    {
        unlock();
    }
}

void LockFile::lock()
{
    if (!m_lockFile.try_lock())
    {
        qCritical("Failed to lock the lockfile.");
        throw LockException();
    }
    else
    {
        qInfo() << "Successful lock on " << QDir::cleanPath(Config::lockFileName);
        m_isLockFileLocal = true;
    }
}

void LockFile::unlock()
{
    qInfo() << "Unlocking the lock file.";
    m_lockFile.unlock();
}

void LockFile::cede()
{
    if (m_isLockFileLocal)
    {
        m_lockFile.unlock();
        QFile cededFile(Config::lockFileName);
        if (!cededFile.open(QIODevice::ReadWrite))
        {
            qCritical("Failed to cede the lock file.");
        }
        cededFile.close();
        m_isLockFileLocal = false;
    }
}

void LockFile::clear()
{
    QFile lockFile(Config::lockFileName);

    if (lockFile.exists())
    {
        lockFile.remove();
    }
}
