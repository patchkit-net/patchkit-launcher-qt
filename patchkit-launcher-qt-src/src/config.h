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

    const static QString lockFileName;

    const static QString applicationDirectoryName;

    const static int minConnectionTimeoutMsec;
    const static int maxConnectionTimeoutMsec;

    const static QString mainApiUrl;
    const static QStringList cacheApiUrls;

    const static QString apiUrlOverrideEnvironmentVariableName;

    const static QString geolocationApiUrl;
    const static int geolocationTimeout;

    const static QString pingTarget;
    const static QString pingCountArg;

    const static int staleLockTime;
    const static int lockingTimeout;

    const static bool    isLockFilePassingEnabled;

    const static char inlineData[];
};
