/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "config.h"

Config::Config(QString t_logFileName,
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
               QStringList t_cacheApiUrls)
    : logFileName(t_logFileName)
    , dataFileName(t_dataFileName)
    , dataResourceId(t_dataResourceId)
    , dataResourceTypeId(t_dataResourceTypeId)
    , patcherDirectoryName(t_patcherDirectoryName)
    , patcherInstallationInfoFileName(t_patcherInstallationInfoFileName)
    , patcherVersionInfoFileName(t_patcherVersionInfoFileName)
    , patcherIdInfoFileName(t_patcherIdInfoFileName)
    , patcherManifestFileName(t_patcherManifestFileName)
    , applicationDirectoryName(t_applicationDirectoryName)
    , minConnectionTimeoutMsec(t_minConnectionTimeoutMsec)
    , maxConnectionTimeoutMsec(t_maxConnectionTimeoutMsec)
    , chunkedDownloadStaleTimeoutMsec(t_chunkedDownloadStaleTimeoutMsec)
    , mainApiUrl(t_mainApiUrl)
    , cacheApiUrls(t_cacheApiUrls)
{
}

const Config Config::standard = Config
(
    "launcher-log.txt",         // logFileName
    "launcher.dat",             // dataFileName
    3151,                       // dataResourceId
    10,                         // dataResourceTypeId
    "patcher",                  // patcherDirectoryName
    "install_info",             // patcherInstallationInfoFileName
    "version_info",             // patcherVersionInfoFileName
    "id_info",                  // patcherIdInfoFileName
    "patcher.manifest",         // patcherManifestFileName
    "app",                      // applicationDirectoryName
    10000,                      // minConnectionTimeoutMsec
    30000,                      // maxConnectionTimeoutMsec
    120000,                     // chunkedDownloadStaleTimeoutMsec
    "http://api.patchkit.net",  // mainApiUrl
    {                           // cacheApiUrls
        "http://api-cache-1.patchkit.net",
        "http://api-cache-2.patchkit.net",
        "http://api-cache-3.patchkit.net"
    }
);
