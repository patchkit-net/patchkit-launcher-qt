/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "mainwindow.h"
#include "launcherthread.h"
#include "patchkitlocalpatcher.h"
#include "patchkitremotepatcher.h"
#include "launcherconfiguration.h"
#include "launcherlog.h"

LauncherConfiguration createLauncherConfiguration()
{
    return LauncherConfiguration("launcher.dat");
}

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);

#ifdef QT_DEBUG
    logDebug("Setting current directory to - %1", .arg(application.applicationDirPath()));
    QDir::setCurrent(application.applicationDirPath());
#endif

    std::shared_ptr<RemotePatcher> remotePatcher(new PatchKitRemotePatcher());
    std::shared_ptr<LocalPatcher> localPatcher(new PatchKitLocalPatcher());
    std::shared_ptr<LauncherThread> launcherThread(new LauncherThread(
        createLauncherConfiguration(),
        remotePatcher,
        localPatcher));

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
        }
    }

    return result;
}
