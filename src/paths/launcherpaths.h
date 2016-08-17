/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include "launcherconfiguration.h"
#include "launcherlog.h"
#include <QDir>
#include <QApplication>

class LauncherPaths
{
public:
    static void initialize();

    static QString applicationFilePath()
    {
        return m_applicationFilePath;
    }

    static QString applicationDirPath()
    {
        return m_applicationDirPath;
    }

    static QString currentDirPath()
    {
        return QDir::currentPath();
    }

    static void setCurrentDirPath(QString t_currentDirPath)
    {
        QDir currentDir(t_currentDirPath);

        if (!currentDir.exists())
        {
            currentDir.mkpath(".");
        }

        QDir::setCurrent(currentDir.path());

        LauncherLog::setupLogFile(logFilePath());
    }

    static QString dataFilePath()
    {
        return QDir::cleanPath(applicationDirPath() + "/" + LauncherConfiguration::dataFileName);
    }

    static QString patcherDirectoryPath()
    {
        return QDir::cleanPath(currentDirPath() + "/" + LauncherConfiguration::patcherDirectoryName);
    }

    static QString patcherInstallationInfoFilePath()
    {
        return QDir::cleanPath(patcherDirectoryPath() + "/" + LauncherConfiguration::patcherDirectoryName);
    }

    static QString patcherVersionInfoFilePath()
    {
        return QDir::cleanPath(patcherDirectoryPath() + "/" + LauncherConfiguration::patcherVersionInfoFileName);
    }

    static QString patcherManifestFilePath()
    {
        return QDir::cleanPath(patcherDirectoryPath() + "/" + LauncherConfiguration::patcherManifestFileName);
    }

    static QString applicationInstallationDirPath()
    {
        return QDir::cleanPath(currentDirPath() + "/" + LauncherConfiguration::applicationDirectoryName);
    }

private:
    static QString m_applicationFilePath;
    static QString m_applicationDirPath;

    static QString logFilePath()
    {
        return QDir::cleanPath(currentDirPath() + "/" + LauncherConfiguration::logFileName);
    }
};
