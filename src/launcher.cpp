/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcher.h"
#include "log/launcherlog.h"
#include "paths/launcherpaths.h"
#include <QMessageBox>

Launcher::Launcher(const QApplication& t_application)
{
    LauncherPaths::initialize();
    connect(&t_application, &QApplication::aboutToQuit, this, &Launcher::cleanup);
}

void Launcher::start()
{
    logInfo("Starting launcher.");

    m_thread = std::make_shared<LauncherThread>();

    m_mainWindow = std::make_unique<MainWindow>(m_thread, nullptr);

    logInfo("Showing main window.");
    m_mainWindow->show();

    logInfo("Starting launcher thread.");
    m_thread->start();

    connect(m_thread.get(), &QThread::finished, this, &Launcher::finish);
}

void Launcher::finish()
{
    disconnect(m_thread.get(), &QThread::finished, this, &Launcher::finish);

    logInfo("Launcher thread has finished. Checking result.");

    if (m_thread->noError())
    {
        logInfo("Launcher thread has no errors! Closing launcher application with status 0.");

        QApplication::quit();
    }
    else
    {
        logWarning("Launcher thread has failed! Asking for retry.");

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
    if (m_thread)
    {
        if (m_thread->isRunning())
        {
            logWarning("Application is about to be closed but launcher thread is still working - cancelling thread and waiting 2s for result.");
            m_thread->cancel();
            if (!m_thread->wait(2000))
            {
                logWarning("Launcher thread couldn't be cancelled - terminating thread.");
                m_thread->terminate();
                m_thread->wait();
            }
        }

        if (!m_thread->noError())
        {
            logCritical("An error has occured!");
        }
    }
}
