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
    : m_worker(m_state, this)
    , m_mainWindow(m_worker, nullptr)
{
    connect(&t_application, &QApplication::aboutToQuit, this, &Launcher::cleanup);
    connect(&m_state, &LauncherState::error, this, &Launcher::onError);
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

void LauncherApp::Launcher::onError(DownloadError t_error)
{
    if (t_error == DownloadError::ConnectionIssues)
    {
        qWarning("Connection issues.");
        if (m_worker.canStartPatcher())
        {
            qInfo("A version of patcher is installed, asking if launcher should go into offline mode.");
            int answer = QMessageBox::question(nullptr, "Connection issues!",
                                  "Launcher is experiencing connection issues, would you like to continue in offline mode?",
                                  QMessageBox::Yes, QMessageBox::No);

            if (answer == QMessageBox::Yes)
            {
                qInfo("User chose to go into offline mode.");
                emit m_state.respond();

                m_worker.stop();
                m_worker.startPatcher(LauncherCore::Types::NetworkStatus::Offline);
            }
            else if (answer == QMessageBox::No)
            {
                qInfo("User chose to continue trying to download the newest version of patcher.");
                emit m_state.respond();
            }
            else
            {
                emit m_state.respond();
                qWarning("Unexpected outcome.");
            }
        }
        else
        {
            qInfo("No version of patcher is installed.");
            int answer = QMessageBox::warning(nullptr, "Connection issues!",
                                  "Launcher is experiencing connection issues, would you like to keep trying?",
                                  QMessageBox::Yes, QMessageBox::No);

            if (answer == QMessageBox::Yes)
            {
                qInfo("User chose to keep trying.");
                emit m_state.respond();
            }
            else if (answer == QMessageBox::No)
            {
                qInfo("User chose to stop.");
                emit m_state.respond();
                m_worker.stop();
            }
        }
    }
}

void LauncherApp::Launcher::finish()
{
    disconnect(&m_worker, &QThread::finished, this, &Launcher::finish);

    qInfo("Launcher worker has finished. Checking result.");

    if (m_worker.result() == LauncherWorker::CANCELLED ||
        m_worker.result() == LauncherWorker::SUCCESS)
    {
        qInfo("Launcher worker has no errors! Closing launcher application with status 0.");

        QApplication::quit();
    }
    else if (m_worker.result() == LauncherWorker::LOCKED)
    {
        qWarning("Launcher is locked by another instance! Notifying the user.");
        QMessageBox::critical(nullptr, "Locked", "Another instance of Launcher is already running.");

        qWarning("Closing launcher with status 1.");
        QApplication::exit(1);
    }
    else if (m_worker.result() == LauncherWorker::FAILED)
    {
        qWarning("Launcher worker has failed! Asking for retry.");

        int retryRunDialogResult = QMessageBox::critical(nullptr, "Error!", "An error has occured! Would you like to retry?", QMessageBox::Yes, QMessageBox::No);

        if (retryRunDialogResult == QMessageBox::Yes)
        {
            qInfo("Retrying.");
            start();
        }
        else
        {
            qWarning("Retry denied. Closing launcher application with status 1.");
            QApplication::exit(1);
        }
    }
    else if (m_worker.result() == LauncherWorker::CONNECTION_ERROR)
    {
        int retryRunDialogResult = QMessageBox::critical(nullptr, "Connection error!", "Failed to connect to server. Would you like to retry?", QMessageBox::Yes, QMessageBox::No);

        if (retryRunDialogResult == QMessageBox::Yes)
        {
            qInfo("Retrying.");
            start();
        }
        else
        {
            qWarning("Retry denied. Closing launcher application with status 1.");
            QApplication::exit(1);
        }
    }
    else if (m_worker.result() == LauncherWorker::FATAL_ERROR)
    {
        QMessageBox::critical(nullptr, "Error!", "An error has occured!", QMessageBox::Close);
        QApplication::exit(1);
    }
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

    if (m_worker.result() == LauncherWorker::FAILED || m_worker.result() == LauncherWorker::FATAL_ERROR)
    {
        qCritical("An error has occured!");
    }
}
