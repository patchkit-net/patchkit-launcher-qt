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

    qDebug("OSX initializing path.");

    QDir genericDataLocation = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);

    QDir patchKitAppsPath = QDir::cleanPath(genericDataLocation.path() + "/" + "PatchKit/Apps");

    QDir appPath = QDir::cleanPath(patchKitAppsPath.path() + "/" + t_data.applicationSecret().mid(0, 8));

    if (!appPath.exists())
    {
        appPath.mkpath(".");
    }

    if (QDir::setCurrent(appPath.path()))
    {
        qDebug("Path set to %1", .arg(appPath.path()));

    }
    else
    {
        qDebug("Couldn't set path to %1", .arg(appPath.path()));

    }

#else

    qDebug("Initializing path.");

    QDir appPath = applicationDirPath();

    qDebug() << "Path set to " << appPath.path();

    QDir::setCurrent(appPath.path());

#endif

#if defined(QT_DEBUG)
    qDebug("Evaulating all the paths:");
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
