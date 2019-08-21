#include "patcher.h"

#include <QDir>

#include "config.h"
#include "ioutils.h"

locations::Patcher::Patcher(const QString& patcherDirectory)
    : m_directory(patcherDirectory)
{
    IOUtils::createDir(m_directory);
}

QString locations::Patcher::directory() const
{
    return m_directory;
}

QString locations::Patcher::installationInfoFile() const
{
    return QDir(m_directory).filePath(Config::patcherInstallationInfoFileName);
}

QString locations::Patcher::versionInfoFile() const
{
    return QDir(m_directory).filePath(Config::patcherVersionInfoFileName);
}

QString locations::Patcher::idInfoFile() const
{
    return QDir(m_directory).filePath(Config::patcherIdInfoFileName);
}

QString locations::Patcher::manifestFile() const
{
    return QDir(m_directory).filePath(Config::patcherManifestFileName);
}

QString locations::Patcher::launcherLocationFile() const
{
    return QDir(m_directory).filePath(Config::patcherLauncherPathFileName);
}
