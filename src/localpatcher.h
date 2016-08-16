/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>
#include <QString>

#include "launcherdata.h"

class LocalPatcher : public QObject
{
    Q_OBJECT

public:
    virtual bool isInstalled() = 0;

    // Can assume that patcher is installed.
    virtual int getVersion() = 0;

    // Can assume that patcher is uninstalled.
    virtual void install(const QString& t_downloadedPath, int t_version) = 0;

    // Cannot assume that patcher is installed.
    virtual void uninstall() = 0;

    // Can assume that patcher is installed.
    virtual void start(const LauncherData& t_data) = 0;

public slots:
    // Cancels any above operation. If cancellation is successful, LauncherCancelledException is thrown by the operation.
    virtual void cancel() = 0;
};
