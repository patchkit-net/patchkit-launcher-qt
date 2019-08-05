/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QStringList>
#include "remote/downloading/timeout.h"

#ifndef CONFIG_INLINE_DATA
#define CONFIG_INLINE_DATA_PLACEHOLDER "OnceUponAMidnightDreary"
#define CONFIG_INLINE_DATA_PLACEHOLDER_HASH "23d361114b8f549eb681c6f9b582edb6"
#define CONFIG_INLINE_DATA_PLACEHOLDER_LENGTH 23
#endif

#ifndef CONFIG_INLINE_DATA_SIZE
#define CONFIG_INLINE_DATA_SIZE 1024
#endif

namespace Globals
{
    enum class Platform
    {
        windows_x86,
        windows_x86_64,
        mac_x86,
        mac_x86_64,
        linux_x86,
        linux_x86_64
    };

    QString toString(Platform platform);

    Platform currentPlatform();
    const QString& currentPlatformString();
    const QString& version();
}

class Config
{
public:
    static bool isSelectableInstallationLocationEnabled();
    const static bool useSelectableInstallationLocation;
    const static QString selectableInstallationLocationVariableName;

    const static QString logFileName;

    const static QString dataFileName;
    const static int dataResourceId;
    const static int dataResourceTypeId;

    const static QString patcherDownloadFileName;

    const static QString patcherDirectoryName;
    const static QString patcherInstallationInfoFileName;
    const static QString patcherVersionInfoFileName;
    const static QString patcherIdInfoFileName;
    const static QString patcherManifestFileName;
    const static QString patcherLauncherPathFileName;
    const static QString locationFileName;

    const static QString lockFileName;

    const static QString applicationDirectoryName;

    const static Timeout minConnectionTimeoutMsec;
    const static Timeout maxConnectionTimeoutMsec;

    const static int maxInvalidChunksCount;

    const static Timeout downloadTimeoutMsec;

    const static QString mainApiUrl;
    const static QStringList cacheApiUrls;

    const static QString apiUrlOverrideEnvironmentVariableName;
    const static QString apiCacheUrlOverrideEnvironmentVariableName;
    const static QString appSecretOverrideEnvVar;
    const static QString appVersionIdOverrideEnvVar;

    const static QString geolocationApiUrl;
    const static Timeout geolocationTimeout;

    const static QString pingTarget;
    const static QString pingCountArg;

    const static char inlineData[];
};
