/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "locations.h"

#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include "config.h"
#include "data/data.h"
#include "logger.h"

bool Locations::isCurrentDirWritable() const
{
    QFile permissionsCheckFile(QDir(currentDirPath()).filePath(".writable_check"));

    if (permissionsCheckFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        permissionsCheckFile.remove();

        return true;
    }

    return false;
}

#if defined(Q_OS_OSX)

QString osxWritableDirectory()
{
    auto sep = QDir::separator();

    auto genericDataLocation = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);

    auto appsDirectory = QDir(genericDataLocation + sep + "PatchKit" + sep + "Apps");

    if (!appsDirectory.exists())
    {
        appsDirectory.mkpath(".");
    }

    return appsDirectory;
}

#endif

Locations::Locations()
{
#if defined(Q_OS_OSX)
    m_logPath = QDir(osxWritableDirectory()).filePath(Config::logFileName);
#else

    m_logPath = QDir(applicationDirPath()).filePath(Config::logFileName);

#endif
}

void Locations::initializeWithData(const Data& t_data)
{
    qDebug("Initializing path.");

#if defined(Q_OS_OSX)

    QDir appPath = QDir(osxWritableDirectory() + QDir::separator() + t_data.applicationSecret().mid(0, 8));

    if (!appPath.exists())
    {
        appPath.mkpath(".");
    }

#else

    QDir appPath = applicationDirPath();

#endif

    if (QDir::setCurrent(appPath.path()))
    {
        qDebug() << "Path set to " << appPath.path();
    }
    else
    {
        qDebug() << "Couldn't set path to " << appPath.path();
    }

#if defined(QT_DEBUG)
    qDebug("Evaulating all the paths:");
    evalutatePaths();
#endif
}

QString Locations::patcherDownloadPath() const
{
    return QDir(currentDirPath()).filePath(Config::patcherDownloadFileName);
}

QString Locations::applicationFilePath() const
{
    return QApplication::applicationFilePath();
}

QString Locations::applicationDirPath() const
{
    return QApplication::applicationDirPath();
}

QString Locations::currentDirPath() const
{
    return QDir::currentPath();
}

QString Locations::logFilePath() const
{
    return m_logPath;
}

QString Locations::dataFilePath() const
{
    return QDir(applicationDirPath()).filePath(Config::dataFileName);
}

QString Locations::patcherDirectoryPath() const
{
    return QDir(currentDirPath()).filePath(Config::patcherDirectoryName);
}

QString Locations::patcherInstallationInfoFilePath() const
{
    return QDir(patcherDirectoryPath()).filePath(Config::patcherInstallationInfoFileName);
}

QString Locations::patcherVersionInfoFilePath() const
{
    return QDir(patcherDirectoryPath()).filePath(Config::patcherVersionInfoFileName);
}

QString Locations::patcherIdInfoFilePath() const
{
    return QDir(patcherDirectoryPath()).filePath(Config::patcherIdInfoFileName);
}

QString Locations::patcherManifestFilePath() const
{
    return QDir(patcherDirectoryPath()).filePath(Config::patcherManifestFileName);
}

QString Locations::patcherLauncherPathFilePath() const
{
    return QDir(patcherDirectoryPath()).filePath(Config::patcherLauncherPathFileName);
}

QString Locations::applicationInstallationDirPath() const
{
    return QDir(currentDirPath()).filePath(Config::applicationDirectoryName);
}

#if defined (QT_DEBUG)
void Locations::evalutatePaths() const
{
    qDebug() << "Application file path " << applicationFilePath();
    qDebug() << "Application dir path " << applicationDirPath();

    qDebug() << "Current dir path " << currentDirPath();

    qDebug() << "Log file path " << logFilePath();
    qDebug() << "Data file path " << dataFilePath();

    qDebug() << "Patcher directory path " << patcherDirectoryPath();
    qDebug() << "Patcher installation info file path " << patcherInstallationInfoFilePath();
    qDebug() << "Patcher version info file path " << patcherVersionInfoFilePath();
    qDebug() << "Patcher if info file path " << patcherIdInfoFilePath();
    qDebug() << "Patcher manifest file path " << patcherManifestFilePath();

    qDebug() << "Application installation dir path " << applicationInstallationDirPath();
}
#endif
