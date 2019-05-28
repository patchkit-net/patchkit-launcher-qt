#include "lockfile.h"

#include <QDebug>
#include <QDir>
#include <QFile>

#include "config.h"
#include "locations.h"
#include "customexceptions.h"

#include <boost/interprocess/exceptions.hpp>

LockFile::LockFile()
    : m_isLockFileLocal(false)
{
    QFile lockFile(Config::lockFileName);
    if (!lockFile.exists())
    {
        lockFile.open(QIODevice::ReadWrite);
        lockFile.close();
    }

    // https://www.boost.org/doc/libs/1_70_0/doc/html/boost/interprocess/file_lock.html#idm46381346245600-bb
    // According to boost documentation, instantiating the file_lock class
    // may fail and throw interprocess_exception if the file does not exist or
    // there are no system resources.
    //
    // For some reason it also seems to fail if the file is locked, hence the try/catch
    try
    {
        m_lockFile = boost::interprocess::file_lock(Config::lockFileName.toStdString().c_str());
    }
    catch (boost::interprocess::interprocess_exception& e)
    {
        qCritical() << "Failed to instantiate lock file: " << e.what();
        throw LockException("Failed to instantiate lock file");
    }
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
    try
    {
        if (!m_lockFile.try_lock())
        {
            qCritical("Failed to lock the lockfile.");
            throw LockException("Failed to lock the lockfile.");
        }
        else
        {
            qInfo() << "Successful lock on " << QDir::cleanPath(Config::lockFileName);
            m_isLockFileLocal = true;
        }
    }
    catch (boost::interprocess::interprocess_exception& e)
    {
        qCritical() << "Locking failed: " << e.what();
        throw LockException("Locking failed");
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
