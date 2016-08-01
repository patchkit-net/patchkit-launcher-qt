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
    explicit MainWindow(QWidget *t_parent = nullptr);
    ~MainWindow();

signals:
    void cancelled();

public slots:
    void cancel();
    void finish();
    void setStatus(const QString& t_status) const;
    void setProgress(int t_progress) const;

protected:
    void showEvent(QShowEvent *t_event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *t_event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *t_event) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *t_event) Q_DECL_OVERRIDE;

private:
    bool m_allowClose;
    int m_bytesDownloaded;
    int m_totalBytes;
    QPoint m_dragPosition;
    Ui::MainWindow * const m_ui;
};

#endif // MAINWINDOW_H
