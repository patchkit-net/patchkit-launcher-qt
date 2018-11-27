/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcher.h"

#include <QMessageBox>
#include <QDebug>

#include <src/config.h>
#include <src/logger.h>
#include <src/locations.h>
#include <src/lockfile.h>

LauncherApp::Launcher::Launcher(const QApplication& t_application)
    : m_interface()
    , m_mainWindow(m_worker, nullptr)
    , m_worker(m_interface, this)
{
    connect(&t_application, &QApplication::aboutToQuit, this, &Launcher::cleanup);
}

void LauncherApp::Launcher::start()
{
    qInfo() << "Starting launcher, version: " << Globals::version();
    connect(&m_worker, &QThread::finished, this, &Launcher::finish);

    qInfo("Showing main window.");
    m_mainWindow.show();

    qInfo("Starting launcher worker.");
    m_worker.start();
}

void LauncherApp::Launcher::finish()
{
    disconnect(&m_worker, &QThread::finished, this, &Launcher::finish);
}

void LauncherApp::Launcher::cleanup()
{
    if (m_worker.isRunning())
    {
        qWarning("Application is about to be closed but launcher thread is still working - cancelling thread and waiting 2s for result.");
        m_worker.cancel();
        if (!m_worker.wait(2000))
        {
            qWarning("Launcher thread couldn't be cancelled - terminating thread.");
            m_worker.terminate();
            m_worker.wait();
        }
    }
}
