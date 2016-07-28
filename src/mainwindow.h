#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ilauncherwindow.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public ILauncherWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setBytesDownloaded(long m_bytesDownloaded) Q_DECL_OVERRIDE;
    void setTotalBytes(long m_totalBytes) Q_DECL_OVERRIDE;
    void setStatus(const QString &status) Q_DECL_OVERRIDE;
    void setProgress(int progress) Q_DECL_OVERRIDE;

protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void refreshDownloadStatus();

    int m_bytesDownloaded;
    int m_totalBytes;
    QPoint m_dragPosition;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
