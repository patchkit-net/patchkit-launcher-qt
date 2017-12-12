#include "lockfile.h"

#include "config.h"
#include "locations.h"
#include "customexceptions.h"

LockFile::LockFile(QObject* parent)
    : QObject(parent)
    , m_lockFile(Config::lockFileName)
    , m_isLockFileLocal(false)
{
}

LockFile::~LockFile()
{
    unlock();
}

void LockFile::lock()
{
    if (!m_lockFile.tryLock(Config::lockingTimeout))
    {
        qCritical("Failed to lock the lockfile.");
        throw LockException();
    }
    else
    {
        m_isLockFileLocal = true;
    }
}

void LockFile::unlock()
{
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
    }
}
