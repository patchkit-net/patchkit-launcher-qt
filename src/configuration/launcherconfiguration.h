/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QString>

class LauncherConfiguration
{
public:
    const static QString logFileName;
    const static QString dataFileName;
    const static int dataResourceId;
    const static int dataResourceTypeId;

    const static QString patcherDirectoryName;
    const static QString patcherInstallationInfoFileName;
    const static QString patcherVersionInfoFileName;
    const static QString patcherManifestFileName;

    const static QString applicationDirectoryName;
};
