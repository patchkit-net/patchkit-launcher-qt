/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcherconfiguration.h"

LauncherConfiguration::LauncherConfiguration(const QString& t_dataFileName, const QString& t_applicationFilePath, int t_dataResourceId, int t_dataResourceTypeId) :
    dataFileName(t_dataFileName),
    applicationFilePath(t_applicationFilePath),
    dataResourceId(t_dataResourceId),
    dataResourceTypeId(t_dataResourceTypeId)
{
}
