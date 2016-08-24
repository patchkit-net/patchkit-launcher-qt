/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QDir>

#include "config.h"
#include "logger.h"

class Locations
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

        Logger::setupLogFile(logFilePath());
    }

    static QString dataFilePath()
    {
        return QDir::cleanPath(applicationDirPath() + "/" + Config::dataFileName);
    }

    static QString patcherDirectoryPath()
    {
        return QDir::cleanPath(currentDirPath() + "/" + Config::patcherDirectoryName);
    }

    static QString patcherInstallationInfoFilePath()
    {
        return QDir::cleanPath(patcherDirectoryPath() + "/" + Config::patcherDirectoryName);
    }

    static QString patcherVersionInfoFilePath()
    {
        return QDir::cleanPath(patcherDirectoryPath() + "/" + Config::patcherVersionInfoFileName);
    }

    static QString patcherIdInfoFilePath()
    {
        return QDir::cleanPath(patcherDirectoryPath() + "/" + Config::patcherIdInfoFileName);
    }

    static QString patcherManifestFilePath()
    {
        return QDir::cleanPath(patcherDirectoryPath() + "/" + Config::patcherManifestFileName);
    }

    static QString applicationInstallationDirPath()
    {
        return QDir::cleanPath(currentDirPath() + "/" + Config::applicationDirectoryName);
    }

private:
    static QString m_applicationFilePath;
    static QString m_applicationDirPath;

    static QString logFilePath()
    {
        return QDir::cleanPath(currentDirPath() + "/" + Config::logFileName);
    }
};
