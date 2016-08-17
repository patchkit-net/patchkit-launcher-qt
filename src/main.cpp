/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcherconfiguration.h"
#include "launcherpaths.h"
#include "mainwindow.h"
#include "launcherthread.h"
#include "launcherlog.h"
#include <QtMessageHandler>
#include <QMessageBox>

int main(int argc, char* argv[])
{
    //TODO: Move it to LauncherApplication.
    QApplication application(argc, argv);

    LauncherPaths::initialize();

    std::shared_ptr<LauncherThread> launcherThread(new LauncherThread());

    MainWindow mainWindow(launcherThread, nullptr);

    logInfo("Showing main window.");
    mainWindow.show();

    launcherThread->start();

    logInfo("Starting application loop.");
    int result = application.exec();

    if (launcherThread->isRunning())
    {
        logWarning("Qt application has been closed but launcher thread is still working - cancelling thread.");
        launcherThread->cancel();
        if (!launcherThread->wait(2000))
        {
            logWarning("Launcher thread couldn't be cancelled - terminating thread.");
            launcherThread->terminate();
            launcherThread->wait();
        }
    }

    if (!launcherThread->noError())
    {
        logCritical("An error has occured!");
        QMessageBox::critical(nullptr, "Error!", "An error has occured!", QMessageBox::Ok, QMessageBox::NoButton);
    }

    return result;
}
