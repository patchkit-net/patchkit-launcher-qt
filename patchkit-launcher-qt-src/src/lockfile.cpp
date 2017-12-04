#include "lockfile.h"

#include "config.h"
#include "locations.h"
#include "customexceptions.h"

LockFile::LockFile()
    : m_lockFile(Config::lockFileName)
    , m_isLockFileLocal(false)
{
}

LockFile::~LockFile()
{
    unlock();
}

void LockFile::lock()
{
    if (isLocked())
    {
        throw LockException();
    }
    else
    {
        if (!m_lockFile.open(QIODevice::ReadWrite))
        {
            throw LockException();
        }
        m_lockFile.close();
        m_isLockFileLocal = true;
    }
}

void LockFile::unlock()
{
    if (isLockedLocally())
    {
        m_lockFile.remove();
        m_isLockFileLocal = false;
    }
}

void LockFile::cede()
{
    m_isLockFileLocal = false;
}

bool LockFile::isLocked() const
{
    return m_lockFile.exists();
}

bool LockFile::isLockedLocally() const
{
    return isLocked() && m_isLockFileLocal;
}

LockFile& LockFile::singleton()
{
    static LockFile lockFile;
    return lockFile;
}
