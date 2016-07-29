#include "mainwindow.h"
#include "launcher.h"
#include "launcherexception.h"
#include "patchkitremotepatcher.h"
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QFuture>

LauncherConfiguration createLauncherConfiguration()
{
    LauncherConfiguration launcherConfiguration;
    launcherConfiguration.dataFileName = "launcher.dat";

    return launcherConfiguration;
}

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    RemotePatcher *remotePatcher = new PatchKitRemotePatcher();

    Launcher launcher(createLauncherConfiguration(), remotePatcher);

    MainWindow mainWindow(nullptr);

    launcher.connect(&launcher, &Launcher::finished, &mainWindow, &MainWindow::finish);
    launcher.connect(&launcher, &Launcher::bytesDownloadedChanged, &mainWindow, &MainWindow::setBytesDownloaded);
    launcher.connect(&launcher, &Launcher::totalBytesChanged, &mainWindow, &MainWindow::setTotalBytes);
    launcher.connect(&launcher, &Launcher::statusChanged, &mainWindow, &MainWindow::setStatus);
    launcher.connect(&launcher, &Launcher::progressChanged, &mainWindow, &MainWindow::setProgress);

    mainWindow.show();
    launcher.start();

    int result = application.exec();

    delete remotePatcher;

    return result;
}
