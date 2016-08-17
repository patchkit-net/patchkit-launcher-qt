/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include "launcherexception.h"

class LauncherCancelledException : public LauncherException
{
public:
    LauncherCancelledException() : LauncherException("Launcher has been cancelled.")
    {
    }
};
