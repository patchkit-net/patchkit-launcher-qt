/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QString>

struct Data;

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

    void initializeWithData(const Data& t_data);

    QString applicationFilePath() const;

    QString applicationDirPath() const;

    QString currentDirPath() const;

    QString logFilePath() const;

    QString dataFilePath() const;

    QString patcherDirectoryPath() const;

    QString patcherInstallationInfoFilePath() const;

    QString patcherVersionInfoFilePath() const;

    QString patcherIdInfoFilePath() const;

    QString patcherManifestFilePath() const;

    QString applicationInstallationDirPath() const;

    bool isCurrentDirWritable() const;

#if defined(QT_DEBUG)
    void evalutatePaths() const;
#endif

private:
    QString m_logPath;
};
