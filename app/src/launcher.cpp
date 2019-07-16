/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcher.h"

#include <QMessageBox>
#include <QDebug>

#include <config.h>
#include <logger.h>
#include <locations.h>
#include <lockfile.h>

LauncherApp::Launcher::Launcher(const QApplication& t_application)
    : m_mainWindow(m_worker, nullptr)
    , m_interface(this)
    , m_worker(m_interface, this)
{
    m_mainWindow.moveToThread(this->thread());

    connect(&t_application, &QApplication::aboutToQuit, this, &Launcher::cleanup);

    // Thread --> UI
    connect(&m_worker, &LauncherWorker::statusChanged, &m_mainWindow, &MainWindow::setStatus);
    connect(&m_worker, &LauncherWorker::progressChanged, &m_mainWindow, &MainWindow::setProgress);
    connect(&m_worker, &LauncherWorker::finished, &m_mainWindow, &MainWindow::close);

    connect(&m_interface, &Interface::shouldStartInOfflineModeSignal,
            &m_mainWindow, &MainWindow::shouldStartInOfflineMode,
            Qt::BlockingQueuedConnection);

    connect(&m_interface, &Interface::shouldRetrySignal,
            &m_mainWindow, &MainWindow::shouldRetry,
            Qt::BlockingQueuedConnection);

    connect(&m_interface, &Interface::showErrorMessage,
            &m_mainWindow, &MainWindow::showErrorMessage,
            Qt::BlockingQueuedConnection);

    connect(&m_interface, &Interface::selectInstallationLocationSignal,
            &m_mainWindow, &MainWindow::selectInstallationLocation,
            Qt::BlockingQueuedConnection);

    // UI --> Thread
    connect(&m_mainWindow, &MainWindow::cancel, &m_worker, &LauncherWorker::cancel);

    // Termination
    connect(&m_worker, &LauncherWorker::finished,
            &t_application, &QApplication::quit);
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
