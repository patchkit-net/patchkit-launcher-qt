/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcher.h"

#include "logger.h"

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);

    Logger::initialize();

    application.setQuitOnLastWindowClosed(false);

    Launcher launcher(application);
    launcher.start();

    logInfo("Starting application loop.");
    return application.exec();
}
