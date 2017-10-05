/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "utilities.h"

#include "locations.h"
#include "config.h"
#include "customexceptions.h"

#if defined(Q_OS_WIN)
#include <Windows.h>
#endif

void Utilities::tryRestartWithHigherPermissions()
{
#if defined(Q_OS_WIN)
    ShellExecute(nullptr, L"runas", Locations::getInstance().applicationFilePath().toStdWString().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    throw CancelledException();
#else
    throw FatalException("Cannot write in current directory.");
#endif
}

bool Utilities::isCurrentDirectoryWritable()
{
    return Locations::getInstance().isCurrentDirWritable();
}
