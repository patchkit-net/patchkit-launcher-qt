/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QString>

struct Data;

class Locations
{
public:
    Locations(Locations const&) = default;
    Locations(const Data& data);

    void initializeWithData(const Data& t_data);

    static QString applicationFilePath();

    static QString applicationDirPath();

    QString currentDirPath() const;

    static QString logFilePath();

    static QString dataFilePath();

    QString patcherDownloadPath() const;

    QString patcherDirectoryPath() const;

    QString patcherInstallationInfoFilePath() const;

    QString patcherVersionInfoFilePath() const;

    QString patcherIdInfoFilePath() const;

    QString patcherManifestFilePath() const;

    QString patcherLauncherPathFilePath() const;

    QString applicationInstallationDirPath() const;

#if defined(QT_DEBUG)
    void evalutatePaths() const;
#endif
};
