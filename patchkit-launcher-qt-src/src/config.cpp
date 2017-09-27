/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "config.h"

const QString Config::logFileName = "launcher-log.txt";

const QString Config::dataFileName = "launcher.dat";
const int Config::dataResourceId = 3151;
const int Config::dataResourceTypeId = 10;

const QString Config::patcherInstallationInfoFileName = "install_info";
const QString Config::patcherVersionInfoFileName = "version_info";
const QString Config::patcherIdInfoFileName = "id_info";
const QString Config::patcherManifestFileName = "patcher.manifest";

#if defined(Q_OS_OSX)
const QString Config::patcherDirectoryName = "Patcher";
const QString Config::applicationDirectoryName = "Data";
#else
const QString Config::patcherDirectoryName = "patcher";
const QString Config::applicationDirectoryName = "app";
#endif

const int Config::minConnectionTimeoutMsec = 10000;
const int Config::maxConnectionTimeoutMsec = 30000;

const QString Config::mainApiUrl = "https://api.patchkit.net";

const QStringList Config::cacheApiUrls = {
    "https://api-cache.patchkit.net",
    "http://api-cache-1.patchkit.net",
    "http://api-cache-2.patchkit.net",
    "http://api-cache-3.patchkit.net"
};

const QString Config::apiUrlOverrideEnvironmentVariableName = "PK_LAUNCHER_API_URL";

const QString Config::geolocationApiUrl = "https://ip2loc.patchkit.net/v1/country";
const int Config::geolocationTimeout = 10000;

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

const QString& Globals::gitVersion()
{
    static QString gv(GIT_VERSION);
    return gv;
}
