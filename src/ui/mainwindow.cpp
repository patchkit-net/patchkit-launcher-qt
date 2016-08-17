/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include <QShowEvent>
#include <QDesktopWidget>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "launcherlog.h"

MainWindow::MainWindow(std::shared_ptr<LauncherThread> t_launcherThread, QWidget* t_parent) :
    QMainWindow(t_parent, Qt::FramelessWindowHint),
    m_launcherThread(t_launcherThread),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    // Thread --> UI
    connect(m_launcherThread.get(), &LauncherThread::statusChanged, this, &MainWindow::setStatus);
    connect(m_launcherThread.get(), &LauncherThread::progressChanged, this, &MainWindow::setProgress);

    connect(m_launcherThread.get(), &LauncherThread::finished, this, &MainWindow::close);

    // Thread <-- UI
    connect(m_ui->cancelButton, &QPushButton::clicked, m_launcherThread.get(), &LauncherThread::cancel);
}

void MainWindow::setStatus(const QString& t_status) const
{
    m_ui->status->setText(t_status);
}

void MainWindow::setProgress(int t_progress) const
{
    m_ui->progressBar->setValue(t_progress);
}

void MainWindow::showEvent(QShowEvent* t_event)
{
    logInfo("Setting launcher window geometry.");

    const QRect availableSize = QApplication::desktop()->availableGeometry(this);
    move((availableSize.width() - width()) / 2, (availableSize.height() - height()) / 2);

    t_event->accept();
}

void MainWindow::mouseMoveEvent(QMouseEvent* t_event)
{
    if (t_event->buttons() & Qt::LeftButton && !m_ui->cancelButton->underMouse())
    {
        move(t_event->globalPos() - m_dragPosition);
        t_event->accept();
    }
}


void MainWindow::mousePressEvent(QMouseEvent* t_event)
{
    if (t_event->button() == Qt::LeftButton && !m_ui->cancelButton->underMouse())
    {
        m_dragPosition = t_event->globalPos() - frameGeometry().topLeft();
        t_event->accept();
    }
}

void MainWindow::closeEvent(QCloseEvent* t_event)
{
    logInfo("Close window request.");
    if (m_launcherThread->isFinished())
    {
        logInfo("Allowing the window to be closed.");
        t_event->accept();
    }
    else
    {
        logInfo("Not allowing window to be closed - launcher thread is still running. Cancelling launcher thread.");
        m_launcherThread->cancel();
        t_event->ignore();
    }
}
