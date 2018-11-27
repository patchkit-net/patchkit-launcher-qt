/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <memory>

#include "mainwindow.h"
#include "interface.h"

#include <src/launcherworker.h>

namespace LauncherApp
{
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
    Interface m_interface;
    MainWindow m_mainWindow;
    LauncherWorker m_worker;
};
}
