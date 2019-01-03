/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcher.h"

#include <logger.h>
#include <lockfile.h>

#include <QMessageBox>
#include <QNetworkProxyFactory>

int main(int argc, char* argv[])
{
    qInfo("Starting the launcher.");
    QApplication application(argc, argv);

    Logger::initialize();

    application.setQuitOnLastWindowClosed(false);

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    LauncherApp::Launcher launcher(application);
    launcher.start();

    qInfo("Starting application loop.");
    return application.exec();
}
