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
#include <QtMessageHandler>
#include <QMessageBox>

LauncherConfiguration createLauncherConfiguration()
{
    return LauncherConfiguration("launcher.dat");
}

void logMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString& msg)
{
    QString txt = msg;
    txt.prepend(" - ");

    QDateTime date;
    txt.prepend(date.currentDateTime().toString());

    if(type == QtDebugMsg)
    {
        txt.prepend("[DEBUG]    ");
    }
    else if(type == QtInfoMsg)
    {
        txt.prepend("[INFO]     ");
    }
    else if(type == QtWarningMsg)
    {
        txt.prepend("[WARNING]  ");
    }
    else if(type == QtCriticalMsg)
    {
        txt.prepend("[CRITICAL] ");
    }
    else if(type == QtFatalMsg)
    {
        txt.prepend("[FATAL]    ");
    }
    else if(type == QtSystemMsg)
    {
        txt.prepend("[SYSTEM]   ");
    }

    QFile logFile("Launcher-log.txt");

    if(logFile.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        QTextStream logStream(&logFile);
        logStream << txt << endl << flush;
        logFile.close();
    }
}

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);

#ifdef Q_OS_MAC || Q_OS_MAC64 || Q_OS_MACX
    QDir applicationDir(application.applicationDirPath());
    applicationDir.cdUp();
    applicationDir.cd("Resources");
    QDir::setCurrent(applicationDir.absolutePath());
#else
#ifdef QT_DEBUG
    QDir::setCurrent(application.applicationDirPath());
#endif
#endif

    qInstallMessageHandler(logMessageHandler);

    logInfo("Current directory - %1", .arg(QDir::current().path()));

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
            launcherThread->wait();
        }
    }

    if(!launcherThread->isSuccess())
    {
        QMessageBox::critical(nullptr, "Error!", "An error has occured!", QMessageBox::Ok, QMessageBox::NoButton);
    }

    return result;
}
