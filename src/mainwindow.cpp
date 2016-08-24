/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "mainwindow.h"

#include <QShowEvent>
#include <QDesktopWidget>

#include "ui_mainwindow.h"
#include "logger.h"

MainWindow::MainWindow(std::shared_ptr<LauncherWorker> t_launcherThread, QWidget* t_parent) :
    QMainWindow(t_parent, Qt::FramelessWindowHint),
    m_launcherWorker(t_launcherThread),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    // Thread --> UI
    connect(m_launcherWorker.get(), &LauncherWorker::statusChanged, this, &MainWindow::setStatus);
    connect(m_launcherWorker.get(), &LauncherWorker::progressChanged, this, &MainWindow::setProgress);

    connect(m_launcherWorker.get(), &LauncherWorker::finished, this, &MainWindow::close);

    // Thread <-- UI
    connect(m_ui->cancelButton, &QPushButton::clicked, m_launcherWorker.get(), &LauncherWorker::cancel);
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
    logInfo("Setting launcher window position.");

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
    if (m_launcherWorker->isFinished())
    {
        logInfo("Allowing the window to be closed.");
        t_event->accept();
    }
    else
    {
        logInfo("Not allowing window to be closed - launcher thread is still running. Cancelling launcher thread.");
        m_launcherWorker->cancel();
        t_event->ignore();
    }
}
