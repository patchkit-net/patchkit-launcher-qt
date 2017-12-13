/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcher.h"

#include <src/logger.h>
#include <src/lockfile.h>

#include <QMessageBox>
#include <QNetworkProxyFactory>

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);

    Logger::initialize();

    application.setQuitOnLastWindowClosed(false);

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    Launcher launcher(application);
    launcher.start();

    qInfo("Starting application loop.");
    return application.exec();
}
