/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcher.h"

#include <QMessageBox>

#include <src/logger.h>
#include <src/locations.h>

Launcher::Launcher(const QApplication& t_application)
{
    connect(&t_application, &QApplication::aboutToQuit, this, &Launcher::cleanup);
}

void Launcher::start()
{
    logInfo("Starting launcher.");

    m_worker = std::make_shared<LauncherWorker>();

    m_mainWindow = std::make_unique<MainWindow>(m_worker, nullptr);

    connect(m_worker.get(), &QThread::finished, this, &Launcher::finish);
    connect(m_worker.get(), &LauncherWorker::downloadError, this, &Launcher::onError);

    connect(this, &Launcher::requestContinue, m_worker.get(), &LauncherWorker::workerContinue);
    connect(this, &Launcher::requestStop, m_worker.get(), &LauncherWorker::workerStop);
    connect(this, &Launcher::requestStop, m_worker.get(), &LauncherWorker::stopUpdate);

    logInfo("Showing main window.");
    m_mainWindow->show();

    logInfo("Starting launcher worker.");
    m_worker->start();
}

void Launcher::onError(DownloadError t_error)
{
    if (t_error == DownloadError::ConnectionIssues)
    {
        logWarning("Connection issues.");
        if (m_worker->isLocalPatcherInstalled())
        {
            logInfo("A version of patcher is installed, asking if launcher should go into offline mode.");
            int answer = QMessageBox::question(nullptr, "Connection issues!",
                                  "Launcher is experiencing connection issues, would you like to continue in offline mode?",
                                  QMessageBox::Yes, QMessageBox::No);

            if (answer == QMessageBox::Yes)
            {
                logInfo("User chose to go into offline mode.");
                emit requestStop();
            }
            else if (answer == QMessageBox::No)
            {
                logInfo("User chose to continue trying to download the newest version of patcher.");
                emit requestContinue();
            }
            else
            {
                logWarning("Unexpected outcome.");
            }
        }
        else
        {
            logInfo("No version of patcher is installed, forcing to proceed.");
            emit requestContinue();
        }
    }
}

void Launcher::finish()
{
    disconnect(m_worker.get(), &QThread::finished, this, &Launcher::finish);

    logInfo("Launcher worker has finished. Checking result.");

    if (m_worker->result() == LauncherWorker::CANCELLED ||
        m_worker->result() == LauncherWorker::SUCCESS)
    {
        logInfo("Launcher worker has no errors! Closing launcher application with status 0.");

        QApplication::quit();
    }
    else if (m_worker->result() == LauncherWorker::FAILED)
    {
        logWarning("Launcher worker has failed! Asking for retry.");

        int retryRunDialogResult = QMessageBox::critical(nullptr, "Error!", "An error has occured! Would you like to retry?", QMessageBox::Yes, QMessageBox::No);

        if (retryRunDialogResult == QMessageBox::Yes)
        {
            logInfo("Retrying.");
            start();
        }
        else
        {
            logWarning("Retry denied. Closing launcher application with status 1.");
            QApplication::exit(1);
        }
    }
    else if (m_worker->result() == LauncherWorker::FATAL_ERROR)
    {
        QMessageBox::critical(nullptr, "Error!", "An error has occured!", QMessageBox::Close);
        QApplication::exit(1);
    }
}

void Launcher::cleanup()
{
    if (m_worker)
    {
        if (m_worker->isRunning())
        {
            logWarning("Application is about to be closed but launcher thread is still working - cancelling thread and waiting 2s for result.");
            m_worker->cancel();
            if (!m_worker->wait(2000))
            {
                logWarning("Launcher thread couldn't be cancelled - terminating thread.");
                m_worker->terminate();
                m_worker->wait();
            }
        }

        if (m_worker->result() == LauncherWorker::FAILED || m_worker->result() == LauncherWorker::FATAL_ERROR)
        {
            logCritical("An error has occured!");
        }
    }
}
