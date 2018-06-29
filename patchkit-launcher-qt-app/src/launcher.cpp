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

Launcher::Launcher(const QApplication& t_application)
    : m_worker(this)
    , m_mainWindow(m_worker, nullptr)
{
    connect(&t_application, &QApplication::aboutToQuit, this, &Launcher::cleanup);
}

void Launcher::start()
{
    qInfo() << "Starting launcher, version: " << Globals::version();
    connect(&m_worker, &QThread::finished, this, &Launcher::finish);
    connect(&m_worker, &LauncherWorker::query, this, &Launcher::workerQuery);

    qInfo("Showing main window.");
    m_mainWindow.show();

    qInfo("Starting launcher worker.");
    m_worker.start();
}

void Launcher::finish()
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

void Launcher::cleanup()
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

void Launcher::workerQuery(const QString& t_title, const QString& t_question)
{
    int ans = QMessageBox::question(nullptr, t_title, t_question, QMessageBox::Yes, QMessageBox::No);
    emit m_worker.queryAnswer(ans == QMessageBox::Yes);
}
