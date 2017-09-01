/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include "ui_mainwindow.h"
#include <src/launcherworker.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(LauncherWorker& t_launcherWorker, QWidget* t_parent);

private slots:
    void setStatus(const QString& t_status) const;
    void setProgress(int t_progress) const;

protected:
    void showEvent(QShowEvent* t_event) override;
    void mouseMoveEvent(QMouseEvent* t_event) override;
    void mousePressEvent(QMouseEvent* t_event) override;
    void closeEvent(QCloseEvent* t_event) override;

private:
    LauncherWorker& m_launcherWorker;

    QPoint m_dragPosition;

    std::unique_ptr<Ui::MainWindow> m_ui;
};
