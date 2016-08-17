/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcherpaths.h"

QString LauncherPaths::m_applicationFilePath;
QString LauncherPaths::m_applicationDirPath;

void LauncherPaths::initialize()
{
    m_applicationFilePath = QApplication::applicationFilePath();
    m_applicationDirPath = QApplication::applicationDirPath();

    setCurrentDirPath(m_applicationDirPath);
}
