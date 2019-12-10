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
#include "utilities.h"

QString unixWritableDirectory()
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

#if defined(Q_OS_OSX) || defined(Q_OS_UNIX)

    QDir appPath = QDir(unixWritableDirectory() + QDir::separator() + data.applicationSecret().mid(0, 8));

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

QString Locations::workingDirectory(const QString appSecret)
{
#if defined(Q_OS_OSX)
    QDir workingDir = QDir(osxWritableDirectory()).filePath(appSecret.mid(0, 8));
#else
    QDir workingDir = QDir(applicationDirPath());
#endif

    if (!workingDir.exists())
    {
        workingDir.mkpath(".");
    }
    QString path = workingDir.absolutePath();

    return workingDir.absolutePath();
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
#if defined(Q_OS_OSX) || defined(Q_OS_UNIX)
    return QDir(unixWritableDirectory()).filePath(Config::logFileName);
#else
    if (Utilities::isDirectoryWritable(applicationDirPath()))
    {
        return QDir(applicationDirPath()).filePath(Config::logFileName);
    } else
    {
        auto location = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
        if (!location.exists()) {
            location.mkpath(".");
        }
        return location.filePath(Config::logFileName);
    }
#endif
}

QString Locations::lockFilePath()
{
    #if defined(Q_OS_OSX)
    return QDir(osxWritableDirectory()).filePath(Config::lockFileName);
#else
    if (Utilities::isDirectoryWritable(applicationDirPath()))
    {
        return QDir(applicationDirPath()).filePath(Config::lockFileName);
    } else
    {
        auto location = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

        if (!location.exists()) {
            location.mkpath(".");
        }
        return location.filePath(Config::lockFileName);
    }
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
