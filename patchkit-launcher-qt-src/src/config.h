/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QStringList>

struct Config
{
    Config( QString t_logFileName,
            QString t_dataFileName,
            int t_dataResourceId,
            int t_dataResourceTypeId,
            QString t_patcherDirectoryName,
            QString t_patcherInstallationInfoFileName,
            QString t_patcherVersionInfoFileName,
            QString t_patcherIdInfoFileName,
            QString t_patcherManifestFileName,
            QString t_applicationDirectoryName,
            int t_minConnectionTimeoutMsec,
            int t_maxConnectionTimeoutMsec,
            int t_chunkedDownloadStaleTimeoutMsec,
            QString t_mainApiUrl,
            QStringList t_cacheApiUrls
            );

    QString logFileName;

    QString dataFileName;
    int     dataResourceId;
    int     dataResourceTypeId;

    QString patcherDirectoryName;
    QString patcherInstallationInfoFileName;
    QString patcherVersionInfoFileName;
    QString patcherIdInfoFileName;
    QString patcherManifestFileName;

    QString applicationDirectoryName;

    int     minConnectionTimeoutMsec;
    int     maxConnectionTimeoutMsec;

    int     chunkedDownloadStaleTimeoutMsec;

    QString     mainApiUrl;
    QStringList cacheApiUrls;

    const static Config standard;
};
