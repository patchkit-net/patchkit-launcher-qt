/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <memory>
#include "mainwindow.h"

class Launcher : public QObject
{
    Q_OBJECT

public:
    Launcher(const QApplication& t_application);

    void start();

private slots:
    void finish();
    void cleanup();

private:
    std::unique_ptr<MainWindow> m_mainWindow;
    std::shared_ptr<LauncherThread> m_thread;
};
