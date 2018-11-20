/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include "data/data.h"
#include "localpatcherdata.h"
#include "cancellation/cancellationtokensource.h"
#include "remote/api/api.h"
#include "lockfile.h"
#include "data/networkstatus.hpp"
#include "ilauncher.h"

#include <QThread>

class LauncherWorker : public QThread
{
    Q_OBJECT

    void run() override;

public:
    enum Result
    {
        NONE,
        RESTART,
        CANCELLED,
        SUCCESS,
        FAILED,
        LOCKED
    };

    LauncherWorker(ILauncherInterface& launcherInterface, QObject* parent = nullptr);

    void cancel();

    Result result() const;

signals:
    void statusChanged(const QString& t_status);
    void progressChanged(int t_progress);

private slots:
    void setDownloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes);

private:
    Result m_result;
    Data resolveData();
    void runWithData(Data& t_data);

    ILauncherInterface& m_launcherInterface;

    void setupPatcherSecret(Data& t_data);

    CancellationTokenSource m_cancellationTokenSource;
};
