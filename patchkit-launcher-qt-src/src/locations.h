/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QDir>

#include "config.h"

class Locations
{
    Locations();
public:
    Locations(Locations const&) = delete;
    void operator=(Locations const&) = delete;

    static Locations& getInstance()
    {
        static Locations instance;

        return instance;
    }

    QString applicationFilePath()
    {
        return m_applicationFilePath;
    }

    QString applicationDirPath()
    {
        return m_applicationDirPath;
    }

    QString currentDirPath()
    {
        return QDir::currentPath();
    }

    bool isCurrentDirWritable();

    QString logFilePath()
    {
        return m_logFilePath;
    }

    QString dataFilePath()
    {
        return QDir::cleanPath(applicationDirPath() + "/" + Config::standard.dataFileName);
    }

    QString patcherDirectoryPath()
    {
        return QDir::cleanPath(currentDirPath() + "/" + Config::standard.patcherDirectoryName);
    }

    QString patcherInstallationInfoFilePath()
    {
        return QDir::cleanPath(patcherDirectoryPath() + "/" + Config::standard.patcherDirectoryName);
    }

    QString patcherVersionInfoFilePath()
    {
        return QDir::cleanPath(patcherDirectoryPath() + "/" + Config::standard.patcherVersionInfoFileName);
    }

    QString patcherIdInfoFilePath()
    {
        return QDir::cleanPath(patcherDirectoryPath() + "/" + Config::standard.patcherIdInfoFileName);
    }

    QString patcherManifestFilePath()
    {
        return QDir::cleanPath(patcherDirectoryPath() + "/" + Config::standard.patcherManifestFileName);
    }

    QString applicationInstallationDirPath()
    {
        return QDir::cleanPath(currentDirPath() + "/" + Config::standard.applicationDirectoryName);
    }

private:
    QString m_applicationFilePath;
    QString m_applicationDirPath;
    QString m_logFilePath;

    void initializeCurrentDirPath();
};
