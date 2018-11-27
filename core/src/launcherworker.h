/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include "data/data.h"
#include "data/launcherresult.h"
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
    CUSTOM_RUNTIME_ERROR(InsufficientPermissions)

    LauncherWorker(ILauncherInterface& launcherInterface, QObject* parent = nullptr);

    void cancel();

signals:
    void statusChanged(const QString& t_status);
    void progressChanged(int t_progress);

private slots:
    void setDownloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes);

private:
    bool runInternal();

    Data resolveData();
    void update(
            const Data& data, const Api& api,
            QNetworkAccessManager& nam,
            CancellationToken cancellationToken);

    bool tryUpdate(const Data& data, const Api& api,
                   QNetworkAccessManager& nam,
                   CancellationToken cancellationToken);

    Data setupPatcherSecret(const Data& data, const Api& api, CancellationToken cancellationToken);

    ILauncherInterface& m_launcherInterface;
    CancellationTokenSource m_cancellationTokenSource;
};
