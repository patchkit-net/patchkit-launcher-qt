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
#include "data.h"
#include "logger.h"

bool Locations::isCurrentDirWritable() const
{
    QFile permissionsCheckFile(QDir::cleanPath(currentDirPath() + "/.writable_check"));

    if (permissionsCheckFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        permissionsCheckFile.remove();

        return true;
    }

    return false;
}

Locations::Locations()
{
#if defined(Q_OS_OSX)
    QDir genericDataLocation = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);

    QDir patchKitAppsPath = QDir::cleanPath(genericDataLocation.path() + "/" + "PatchKit/Apps");

    if (!patchKitAppsPath.exists())
    {
        patchKitAppsPath.mkpath(".");
    }

    m_logPath = QDir::cleanPath(patchKitAppsPath.path() + "/" + Config::logFileName);
#else

    m_logPath = QDir::cleanPath(applicationDirPath() + "/" + Config::logFileName);

#endif
}

void Locations::initializeWithData(const Data& t_data)
{
#if defined(Q_OS_OSX)

    logDebug("OSX initializing path.");

    QDir genericDataLocation = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);

    QDir patchKitAppsPath = QDir::cleanPath(genericDataLocation.path() + "/" + "PatchKit/Apps");

    QDir appPath = QDir::cleanPath(patchKitAppsPath.path() + "/" + t_data.applicationSecret().mid(0, 8));

    if (!appPath.exists())
    {
        appPath.mkpath(".");
    }

    if (QDir::setCurrent(appPath.path()))
    {
        logDebug("Path set to %1", .arg(appPath.path()));

    }
    else
    {
        logDebug("Couldn't set path to %1", .arg(appPath.path()));

    }

#else

    logDebug("Initializing path.");

    QDir appPath = applicationDirPath();

    logDebug("Path set to %1", .arg(appPath.path()));

    QDir::setCurrent(appPath.path());

#endif

#if defined(QT_DEBUG)
    logDebug("Evaulating all the paths:");
    evalutatePaths();
#endif
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
    return QDir::cleanPath(applicationDirPath() + "/" + Config::dataFileName);
}

QString Locations::patcherDirectoryPath() const
{
    return QDir::cleanPath(currentDirPath() + "/" + Config::patcherDirectoryName);
}

QString Locations::patcherInstallationInfoFilePath() const
{
    return QDir::cleanPath(patcherDirectoryPath() + "/" + Config::patcherDirectoryName);
}

QString Locations::patcherVersionInfoFilePath() const
{
    return QDir::cleanPath(patcherDirectoryPath() + "/" + Config::patcherVersionInfoFileName);
}

QString Locations::patcherIdInfoFilePath() const
{
    return QDir::cleanPath(patcherDirectoryPath() + "/" + Config::patcherIdInfoFileName);
}

QString Locations::patcherManifestFilePath() const
{
    return QDir::cleanPath(patcherDirectoryPath() + "/" + Config::patcherManifestFileName);
}

QString Locations::applicationInstallationDirPath() const
{
    return QDir::cleanPath(currentDirPath() + "/" + Config::applicationDirectoryName);
}

#if defined (QT_DEBUG)
void Locations::evalutatePaths() const
{
    logDebug("Application file path %1", .arg(applicationFilePath()));
    logDebug("Application dir path %1", .arg(applicationDirPath()));

    logDebug("Current dir path %1", .arg(currentDirPath()));

    logDebug("Log file path %1", .arg(logFilePath()));
    logDebug("Data file path %1", .arg(dataFilePath()));

    logDebug("Patcher directory path %1", .arg(patcherDirectoryPath()));
    logDebug("Patcher installation info file path %1", .arg(patcherInstallationInfoFilePath()));
    logDebug("Patcher version info file path %1", .arg(patcherVersionInfoFilePath()));
    logDebug("Patcher if info file path %1", .arg(patcherIdInfoFilePath()));
    logDebug("Patcher manifest file path %1", .arg(patcherManifestFilePath()));

    logDebug("Application installation dir path %1", .arg(applicationInstallationDirPath()));
}
#endif
