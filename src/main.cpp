/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcher.h"

#include "logger.h"

int main(int argc, char* argv[])
{
    logInfo("Starting application.");

    QApplication application(argc, argv);
    application.setQuitOnLastWindowClosed(false);

    Launcher launcher(application);
    launcher.start();

    logInfo("Starting application loop.");
    return application.exec();
}
