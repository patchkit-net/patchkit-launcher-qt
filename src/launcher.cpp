/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcher.h"

#include <QMessageBox>

#include "logger.h"
#include "locations.h"

Launcher::Launcher(const QApplication& t_application)
{
    Locations::initialize();
    connect(&t_application, &QApplication::aboutToQuit, this, &Launcher::cleanup);
}

void Launcher::start()
{
    logInfo("Starting launcher.");

    m_worker = std::make_shared<LauncherWorker>();

    m_mainWindow = std::make_unique<MainWindow>(m_worker, nullptr);

    connect(m_worker.get(), &QThread::finished, this, &Launcher::finish);

    logInfo("Showing main window.");
    m_mainWindow->show();

    logInfo("Starting launcher worker.");
    m_worker->start();
}

void Launcher::finish()
{
    disconnect(m_worker.get(), &QThread::finished, this, &Launcher::finish);

    logInfo("Launcher worker has finished. Checking result.");

    if (m_worker->noError())
    {
        logInfo("Launcher worker has no errors! Closing launcher application with status 0.");

        QApplication::quit();
    }
    else
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

        if (!m_worker->noError())
        {
            logCritical("An error has occured!");
        }
    }
}
