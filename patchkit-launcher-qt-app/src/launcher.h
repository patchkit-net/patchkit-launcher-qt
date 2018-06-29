/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <memory>

#include "mainwindow.h"
#include <src/launcherworker.h>

class Launcher : public QObject
{
    Q_OBJECT

public:
    Launcher(const QApplication& t_application);

    void start();

private slots:
    void finish();
    void cleanup();

    void workerQuery(const QString& t_title, const QString& t_question);

private:
    LauncherWorker m_worker;
    MainWindow m_mainWindow;
};
