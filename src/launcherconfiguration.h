/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#ifndef LAUNCHERCONFIGURATION_H
#define LAUNCHERCONFIGURATION_H

#include <QString>

struct LauncherConfiguration
{
    explicit LauncherConfiguration(const QString& t_dataFileName);

    const QString dataFileName;
};

#endif // LAUNCHERCONFIGURATION_H
