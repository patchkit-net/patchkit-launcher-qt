#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

void MainWindow::setBytesDownloaded(long bytesDownloaded)
{
    this->m_bytesDownloaded = bytesDownloaded;
    refreshDownloadStatus();
}

void MainWindow::setTotalBytes(long totalBytes)
{
    this->m_totalBytes = totalBytes;
    refreshDownloadStatus();
}

void MainWindow::refreshDownloadStatus()
{
    ui->downloadStatus->setText(tr("Downloading %1 / %2 KB").arg(QString::number(m_bytesDownloaded), QString::number(m_totalBytes)));
}

void MainWindow::setStatus(const QString &status)
{
    ui->status->setText(status);
}

void MainWindow::setProgress(int progress)
{
    ui->progressBar->setValue(progress);
}

MainWindow::~MainWindow()
{
    delete ui;
}
