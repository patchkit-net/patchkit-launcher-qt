/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcher.h"
#include "lockfile.h"

#include <src/logger.h>

#include <QMessageBox>
#include <QNetworkProxyFactory>

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);

    LockFile lockFile;

    try
    {
        lockFile.lock();
    }
    catch(...)
    {
        QMessageBox::critical(nullptr, "Another instance detected.", "An instance of Launcher is already running.", QMessageBox::Ok);
        return -1;
    }

    Logger::initialize();

    application.setQuitOnLastWindowClosed(false);

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    Launcher launcher(application);
    launcher.start();

    qInfo("Starting application loop.");
    return application.exec();
}
