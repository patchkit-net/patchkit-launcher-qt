#include "lockfile.h"

#include "config.h"
#include "locations.h"
#include "customexceptions.h"

LockFile::LockFile()
    : m_lockFile(Config::lockFileName)
    , m_locked(false)
{
}

LockFile::~LockFile()
{
    if (isLockedLocally())
    {
        unlock();
    }
}

void LockFile::lock()
{
    if (m_lockFile.exists())
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
        m_locked = true;
    }
}

void LockFile::unlock()
{
    if (m_lockFile.exists())
    {
        m_lockFile.remove();
    }
    m_locked = false;
}

bool LockFile::isLocked() const
{
    return m_lockFile.exists();
}

bool LockFile::isLockedLocally() const
{
    return isLocked() && m_locked;
}

LockFile& LockFile::singleton()
{
    static LockFile lockFile;
    return lockFile;
}
