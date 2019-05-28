/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "mainwindow.h"

#include <QShowEvent>
#include <QDesktopWidget>
#include <QMessageBox>

#include "ui_mainwindow.h"
#include "interface.h"

#include <logger.h>

MainWindow::MainWindow(LauncherWorker& t_launcherWorker, QWidget* t_parent)
    : QMainWindow(t_parent, Qt::FramelessWindowHint)
    , m_launcherWorker(t_launcherWorker)
{
    m_ui.setupUi(this);

    connect(m_ui.cancelButton, &QPushButton::clicked, this, &MainWindow::cancel);
}

void MainWindow::setStatus(const QString& t_status) const
{
    m_ui.status->setText(t_status);
}

void MainWindow::setProgress(int t_progress) const
{
    m_ui.progressBar->setValue(t_progress);
}

void MainWindow::shouldStartInOfflineMode(ILauncherInterface::OfflineModeAnswer& ans)
{
    int answer = QMessageBox::question(nullptr, "Offline mode?",
        "Launcher had some trouble updating. Would you like to continue in offline mode?",
        QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);

    switch (answer)
    {
    case QMessageBox::Yes:
        ans = ILauncherInterface::OfflineModeAnswer::YES;
        break;

    case QMessageBox::No:
        ans = ILauncherInterface::OfflineModeAnswer::NO;
        break;

    case QMessageBox::Cancel:
        ans = ILauncherInterface::OfflineModeAnswer::CANCEL;
        break;

    default:
        ans = ILauncherInterface::OfflineModeAnswer::NO;
        break;
    }
}

void MainWindow::showErrorMessage(const QString& msg)
{
    QMessageBox::critical(nullptr, "Error", msg);
}

void MainWindow::shouldRetry(const QString& reason, bool& ans)
{
    QString message = QString("%1\nWould you like to retry?").arg(reason);
    int answer = QMessageBox::critical(nullptr, "Error",
        message,
        QMessageBox::Yes, QMessageBox::No);

    ans = answer == QMessageBox::Yes;
}

void MainWindow::showEvent(QShowEvent* t_event)
{
    qInfo("Setting launcher window position.");

    const QRect availableSize = QApplication::desktop()->availableGeometry(this);
    move((availableSize.width() - width()) / 2, (availableSize.height() - height()) / 2);

    t_event->accept();
}

void MainWindow::mouseMoveEvent(QMouseEvent* t_event)
{
    if (t_event->buttons() & Qt::LeftButton && !m_ui.cancelButton->underMouse())
    {
        move(t_event->globalPos() - m_dragPosition);
        t_event->accept();
    }
}


void MainWindow::mousePressEvent(QMouseEvent* t_event)
{
    if (t_event->button() == Qt::LeftButton && !m_ui.cancelButton->underMouse())
    {
        m_dragPosition = t_event->globalPos() - frameGeometry().topLeft();
        t_event->accept();
    }
}

void MainWindow::closeEvent(QCloseEvent* t_event)
{
    qInfo("Close window request.");
    if (m_launcherWorker.isFinished())
    {
        qInfo("Allowing the window to be closed.");
        t_event->accept();
    }
    else
    {
        qInfo("Not allowing window to be closed - launcher thread is still running. Cancelling launcher thread.");
        m_launcherWorker.cancel();
        t_event->ignore();
    }
}
