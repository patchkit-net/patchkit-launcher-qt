/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "locations.h"

#include <QApplication>
#include <QStandardPaths>

Locations::Locations()
{
    m_applicationFilePath = QApplication::applicationFilePath();
    m_applicationDirPath = QApplication::applicationDirPath();

    initializeCurrentDirPath();
}

bool Locations::isCurrentDirWritable()
{
    QFile permissionsCheckFile(QDir::cleanPath(currentDirPath() + "/.writable_check"));

    if (permissionsCheckFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        permissionsCheckFile.remove();

        return true;
    }

    return false;
}

void Locations::initializeCurrentDirPath()
{
    QDir currentDir;

#if defined(Q_OS_OSX)
        currentDir = QDir(applicationDirPath());
        currentDir.cdUp();

        if (!currentDir.exists("Resources"))
        {
            currentDir.mkdir("Resources");
        }
        currentDir.cd("Resources");
#elif defined(Q_OS_WIN)
    currentDir = QDir(applicationDirPath());
#else
        currentDir = QDir(applicationDirPath());
#endif

    if (!currentDir.exists())
    {
        currentDir.mkpath(".");
    }

    QDir::setCurrent(currentDir.path());

    if (isCurrentDirWritable())
    {
        m_logFilePath = QDir::cleanPath(currentDir.path() + "/" + Config::logFileName);
    }
    else
    {
        /*
         * Locations provided by QStandardPaths::DataLocation.
         * Windows - C:/Users/<USER>/AppData/Local/<APPNAME>
         * OSX     - ~/Library/Application Support/<APPNAME>
         * Linux   - ~/.local/share/<APPNAME>
         */
        QDir writableDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);

        if (!writableDir.exists())
        {
            writableDir.mkpath(".");
        }

        m_logFilePath = QDir::cleanPath(writableDir.path() + "/" + Config::logFileName);
    }
}
