#include "lockfile.h"

#include "src/config.h"

LockFile::LockFile()
    : m_lockFile(Config::lockFileName)
    , m_locked(false)
{
}

LockFile::~LockFile()
{
    if (isLocked())
    {
        unlock();
    }
}

void LockFile::lock()
{
    if (m_lockFile.exists())
    {
        throw std::runtime_error("Lock file exists.");
    }
    else
    {
        qInfo("Creating a lock file.");
        if (!m_lockFile.open(QIODevice::ReadWrite))
        {
            throw std::runtime_error("Cannot create or open lock file.");
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
    return m_lockFile.exists() && m_locked;
}
