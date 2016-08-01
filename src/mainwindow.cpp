#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QShowEvent>
#include <QApplication>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *t_parent) :
    QMainWindow(t_parent, Qt::FramelessWindowHint),
    m_bytesDownloaded(0),
    m_totalBytes(0),
    m_ui(new Ui::MainWindow),
    m_allowClose(false)
{
    m_ui->setupUi(this);
    connect(m_ui->cancelButton, &QPushButton::clicked, this, &MainWindow::cancel);
}

MainWindow::~MainWindow()
{
    if(m_ui != nullptr)
    {
        delete m_ui;
    }
}

void MainWindow::cancel()
{
    emit cancelled();
}

void MainWindow::finish()
{
    m_allowClose = true;
    close();
}

void MainWindow::setStatus(const QString& t_status) const
{
    m_ui->status->setText(t_status);
}

void MainWindow::setProgress(int t_progress) const
{
    m_ui->progressBar->setValue(t_progress);
}

void MainWindow::showEvent(QShowEvent *t_event)
{
    const QRect availableSize = QApplication::desktop()->availableGeometry(this);
    move((availableSize.width() - width()) / 2, (availableSize.height() - height()) / 2);

    t_event->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *t_event)
{
    if (t_event->button() == Qt::LeftButton && !m_ui->cancelButton->underMouse())
    {
        m_dragPosition = t_event->globalPos() - frameGeometry().topLeft();
        t_event->accept();
    }
}

void MainWindow::closeEvent(QCloseEvent* t_event)
{
    if(m_allowClose)
    {
        t_event->accept();
    }
    else
    {
        cancel();
        t_event->ignore();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *t_event)
{
    if (t_event->buttons() & Qt::LeftButton && !m_ui->cancelButton->underMouse())
    {
        move(t_event->globalPos() - m_dragPosition);
        t_event->accept();
    }
}