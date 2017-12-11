#include "lockfile.h"

#include "config.h"
#include "locations.h"
#include "customexceptions.h"

#if defined (Q_OS_UNIX)

#include <sys/file.h>
#include <errno.h>

#endif

LockFile::LockFile()
    : m_lockFile(Config::lockFileName)
    , m_isLockFileLocal(false)
{
}

LockFile::~LockFile()
{
    unlock();
}

bool system_lock(int fhandle)
{
#if defined(Q_OS_UNIX)
    if (flock(fhandle, LOCK_EX | LOCK_NB) != 0)
    {
        auto err = errno;

        if (err == EWOULDBLOCK)
        {
            return false;
        }
    }
#endif

    return true;
}

void system_unlock(int fhandle)
{
#if defined(Q_OS_UNIX)
    flock(fhandle, LOCK_UN);
#endif
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

        if (!system_lock(m_lockFile.handle()))
        {
            throw LockException();
        }

        m_isLockFileLocal = true;
    }
}

void LockFile::unlock()
{
    if (isLockedLocally())
    {
        system_unlock(m_lockFile.handle());
        m_lockFile.close();
        m_lockFile.remove();
        m_isLockFileLocal = false;
    }
}

void LockFile::cede()
{
    system_unlock(m_lockFile.handle());
    m_lockFile.close();
    m_isLockFileLocal = false;
}

bool LockFile::isLocked() const
{
    if (!m_lockFile.exists())
    {
        qInfo("Lock file doesn't exist - not locked.");
        return false;
    }

    if (isLockedLocally())
    {
        qWarning("Lock file is locked by this program - locked.");
        return true;
    }

    bool lockResult = system_lock(m_lockFile.handle());
    if (lockResult)
    {
        qDebug("flock successful - not locked.");
        system_unlock(m_lockFile.handle());
        return false;
    }
    else
    {
        qDebug("flock unsuccessful - locked.");
        return true;
    }

    qDebug("Default - not locked");
    return false;
}

bool LockFile::isLockedLocally() const
{
    return m_isLockFileLocal;
}

LockFile& LockFile::singleton()
{
    static LockFile lockFile;
    return lockFile;
}
