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

QString osxWritableDirectory()
{
    auto sep = QDir::separator();

    auto genericDataLocation = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);

    auto appsDirectory = QDir(genericDataLocation + sep + "PatchKit" + sep + "Apps");

    if (!appsDirectory.exists())
    {
        appsDirectory.mkpath(".");
    }

    return appsDirectory.path();
}

Locations::Locations(const Data& data)
{
    qDebug("Initializing path.");

#if defined(Q_OS_OSX)

    QDir appPath = QDir(osxWritableDirectory() + QDir::separator() + data.applicationSecret().mid(0, 8));

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

QString Locations::applicationFilePath()
{
    return QApplication::applicationFilePath();
}

QString Locations::applicationDirPath()
{
    return QApplication::applicationDirPath();
}

QString Locations::currentDirPath() const
{
    return QDir::currentPath();
}

QString Locations::logFilePath()
{
#if defined(Q_OS_OSX)
    return QDir(osxWritableDirectory()).filePath(Config::logFileName);
#else
    return QDir(applicationDirPath()).filePath(Config::logFileName);
#endif
}

QString Locations::dataFilePath()
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
