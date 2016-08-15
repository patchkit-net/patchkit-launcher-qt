/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#ifndef LAUNCHERCONFIGURATION_H
#define LAUNCHERCONFIGURATION_H

#include <QString>

struct LauncherConfiguration
{
    explicit LauncherConfiguration(const QString& t_dataFileName, const QString& t_applicationFilePath, int t_dataResourceId, int t_dataResourceTypeId);

    const QString dataFileName;
    const QString applicationFilePath;
    const int dataResourceId;
    const int dataResourceTypeId;
};

#endif // LAUNCHERCONFIGURATION_H
