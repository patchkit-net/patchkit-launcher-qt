#include "mainwindow.h"
#include "launcher.h"
#include "patchkitremotepatcher.h"
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>

LauncherConfiguration createLauncherConfiguration()
{
    return LauncherConfiguration("launcher.dat");
}

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

#ifdef QT_DEBUG
    QDir::setCurrent(application.applicationDirPath());
#endif

    std::auto_ptr<RemotePatcher> remotePatcher(new PatchKitRemotePatcher());

    Launcher launcher(createLauncherConfiguration(), remotePatcher.get());

    MainWindow mainWindow(nullptr);

    launcher.connect(&launcher, &Launcher::finished, &mainWindow, &MainWindow::finish);
    launcher.connect(&launcher, &Launcher::statusChanged, &mainWindow, &MainWindow::setStatus);
    launcher.connect(&launcher, &Launcher::progressChanged, &mainWindow, &MainWindow::setProgress);
    mainWindow.connect(&mainWindow, &MainWindow::cancelled, &launcher, &Launcher::cancel);

    mainWindow.show();
    launcher.start();

    return application.exec();
}
