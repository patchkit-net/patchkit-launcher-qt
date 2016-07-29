#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>
#include <QShowEvent>
#include <QApplication>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint),
    ui(new Ui::MainWindow),
    m_bytesDownloaded(0),
    m_totalBytes(0)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::finish()
{
    close();
}

void MainWindow::setBytesDownloaded(const long& bytesDownloaded)
{
    m_bytesDownloaded = bytesDownloaded;
    refreshDownloadStatus();
}

void MainWindow::setTotalBytes(const long& totalBytes)
{
    m_totalBytes = totalBytes;
    refreshDownloadStatus();
}

void MainWindow::setStatus(const QString &status)
{
    ui->status->setText(status);
}

void MainWindow::setProgress(const int& progress)
{
    ui->progressBar->setValue(progress);
}

void MainWindow::showEvent(QShowEvent *event)
{
    const QRect availableSize = QApplication::desktop()->availableGeometry(this);
    move((availableSize.width() - width()) / 2, (availableSize.height() - height()) / 2);

    event->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void MainWindow::refreshDownloadStatus()
{
    ui->downloadStatus->setText(tr("Downloading %1 / %2 KB").arg(QString::number(m_bytesDownloaded), QString::number(m_totalBytes)));
}

