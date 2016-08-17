/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include "ui_mainwindow.h"
#include "launcherthread.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(std::shared_ptr<LauncherThread> t_launcherThread, QWidget* t_parent);

private slots:
    void setStatus(const QString& t_status) const;
    void setProgress(int t_progress) const;

protected:
    void showEvent(QShowEvent* t_event) override;
    void mouseMoveEvent(QMouseEvent* t_event) override;
    void mousePressEvent(QMouseEvent* t_event) override;
    void closeEvent(QCloseEvent* t_event) override;

private:
    std::shared_ptr<LauncherThread> m_launcherThread;

    QPoint m_dragPosition;

    std::unique_ptr<Ui::MainWindow> m_ui;
};
