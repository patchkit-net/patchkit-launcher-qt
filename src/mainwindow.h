#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void finish();
    void setBytesDownloaded(const long& bytesDownloaded);
    void setTotalBytes(const long& totalBytes);
    void setStatus(const QString& status);
    void setProgress(const int& progress);

protected:
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void refreshDownloadStatus();

    int m_bytesDownloaded;
    int m_totalBytes;
    QPoint m_dragPosition;
    Ui::MainWindow * const ui;
};

#endif // MAINWINDOW_H
