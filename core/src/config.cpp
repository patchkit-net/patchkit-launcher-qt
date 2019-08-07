/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "config.h"
#include <QProcessEnvironment>

const QString Config::logFileName = "launcher-log.txt";

const QString Config::dataFileName = "launcher.dat";
const int Config::dataResourceId = 3151;
const int Config::dataResourceTypeId = 10;

const QString Config::patcherDownloadFileName = "patcher.zip";

const QString Config::patcherInstallationInfoFileName = "install_info";
const QString Config::patcherVersionInfoFileName = "version_info";
const QString Config::patcherIdInfoFileName = "id_info";
const QString Config::patcherManifestFileName = "patcher.manifest";
const QString Config::patcherLauncherPathFileName = "launcher_path";
const QString Config::locationFileName = ".locations";

const QString Config::lockFileName = ".lock";

#if defined(Q_OS_OSX)
const QString Config::patcherDirectoryName = "Patcher";
const QString Config::applicationDirectoryName = "Data";
#else
const QString Config::patcherDirectoryName = "patcher";
const QString Config::applicationDirectoryName = "app";
#endif

const Timeout Config::minConnectionTimeoutMsec = Timeout::seconds(10);
const Timeout Config::maxConnectionTimeoutMsec = Timeout::seconds(30);
const int Config::maxInvalidChunksCount    = 10;
const Timeout Config::downloadTimeoutMsec  = Timeout::seconds(5);

const QString Config::mainApiUrl = "https://api2.patchkit.net";

const QStringList Config::cacheApiUrls = {
    "https://api-cache.patchkit.net"
};

const QString Config::apiUrlOverrideEnvironmentVariableName = "PK_LAUNCHER_API_URL";
const QString Config::apiCacheUrlOverrideEnvironmentVariableName = "PK_LAUNCHER_API_CACHE_URL";
const QString Config::patcherSecretOverrideEnvVar = "PK_LAUNCHER_FORCE_PATCHER_SECRET";
const QString Config::patcherVersionIdOverrideEnvVar = "PK_LAUNCHER_FORCE_PATCHER_VERSION_ID";

const QString Config::geolocationApiUrl = "https://ip2loc.patchkit.net";
const Timeout Config::geolocationTimeout = Timeout::seconds(10);

const QString Config::pingTarget = "8.8.8.8";

const char Config::inlineData[CONFIG_INLINE_DATA_SIZE] = CONFIG_INLINE_DATA_PLACEHOLDER;

#if defined(_WIN64) || defined(_WIN32)
const QString Config::pingCountArg = "-n";
#else
const QString Config::pingCountArg = "-c";
#endif

QString Globals::toString(Globals::Platform platform)
{
    switch (platform)
    {
    case Platform::windows_x86:
        return "windows_x86";
    case Platform::windows_x86_64:
        return "windows_x86_64";
    case Platform::mac_x86:
        return "mac_x86";
    case Platform::mac_x86_64:
        return "mac_x86_64";
    case Platform::linux_x86:
        return "linux_x86";
    case Platform::linux_x86_64:
        return "linux_x86_64";
    default:
        return "";
    }
}

Globals::Platform Globals::currentPlatform()
{
    static Platform currentPlatformValue;
#if defined(Q_OS_OSX)
    #if defined(Q_PROCESSOR_X86_64)
        currentPlatformValue = Platform::mac_x86_64;
    #else
        currentPlatformValue = Platform::mac_x86;
    #endif
#elif defined (Q_OS_LINUX)
    #if defined(Q_PROCESSOR_X86_64)
        currentPlatformValue = Platform::linux_x86_64;
    #else
        currentPlatformValue = Platform::linux_x86;
    #endif
#else
    #if defined(Q_PROCESSOR_X86_64)
        currentPlatformValue = Platform::windows_x86_64;
    #else
        currentPlatformValue = Platform::windows_x86;
    #endif
#endif

    return currentPlatformValue;
}

const QString& Globals::currentPlatformString()
{
    static QString currentPlatformString = Globals::toString(currentPlatform());
    return currentPlatformString;
}

const QString& Globals::version()
{
    static QString v(VERSION);
    return v;
}

#if defined(Q_OS_OSX)
const bool Config::useSelectableInstallationLocation = true;
#else
const bool Config::useSelectableInstallationLocation = false;
#endif

const QString Config::selectableInstallationLocationVariableName = "PK_LAUNCHER_SELECT_INSTALLATION_LOCATION";

bool Config::isSelectableInstallationLocationEnabled()
{
    QProcessEnvironment env;

    if (env.contains(selectableInstallationLocationVariableName))
    {
        return env.value(selectableInstallationLocationVariableName) == "1";
    }

    return Config::useSelectableInstallationLocation;
}
