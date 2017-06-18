/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QStringList>

#ifndef CONFIG_INLINE_DATA
#define CONFIG_INLINE_DATA_PLACEHOLDER "OnceUponAMidnightDreary"
#define CONFIG_INLINE_DATA_PLACEHOLDER_HASH "23d361114b8f549eb681c6f9b582edb6"
#define CONFIG_INLINE_DATA_PLACEHOLDER_LENGTH 23
#endif

#ifndef CONFIG_INLINE_DATA_SIZE
#define CONFIG_INLINE_DATA_SIZE 1024
#endif

class Config
{
public:
    const static QString logFileName;

    const static QString dataFileName;
    const static int dataResourceId;
    const static int dataResourceTypeId;

    const static QString patcherDirectoryName;
    const static QString patcherInstallationInfoFileName;
    const static QString patcherVersionInfoFileName;
    const static QString patcherIdInfoFileName;
    const static QString patcherManifestFileName;

    const static QString applicationDirectoryName;

    const static int minConnectionTimeoutMsec;
    const static int maxConnectionTimeoutMsec;

    const static int chunkedDownloadStaleTimeoutMsec;

    const static int timeBetweenContentUrlsIterations;

    const static QString mainApiUrl;
    const static QStringList cacheApiUrls;

    const static QString pingTarget;
    const static QString pingCountArg;

    const static char inlineData[];
};
