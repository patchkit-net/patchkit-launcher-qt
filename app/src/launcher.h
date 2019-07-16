/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <memory>

#include "mainwindow.h"
#include "installationdialog.h"
#include "interface.h"

#include <launcherworker.h>

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
    MainWindow m_mainWindow;
    Interface m_interface;
    LauncherWorker m_worker;
};
}
