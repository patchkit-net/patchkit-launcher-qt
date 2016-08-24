/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "locations.h"

#include <QApplication>

QString Locations::m_applicationFilePath;
QString Locations::m_applicationDirPath;

void Locations::initialize()
{
    m_applicationFilePath = QApplication::applicationFilePath();
    m_applicationDirPath = QApplication::applicationDirPath();

    setCurrentDirPath(m_applicationDirPath);
}
