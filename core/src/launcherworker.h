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
#include "networktest.h"

#include <QThread>

using remote::api::Api;

class LauncherWorker : public QThread
{
    Q_OBJECT

    void run() override;

    enum class Action
    {
        RETRY,
        GO_OFFLINE,
        QUIT,
    };

public:
    CUSTOM_RUNTIME_ERROR(InsufficientPermissions)
    CUSTOM_RUNTIME_ERROR(UpdateFailed)

    LauncherWorker(ILauncherInterface& launcherInterface, QObject* parent = nullptr);

    void cancel();

signals:
    void statusChanged(const QString& t_status);
    void progressChanged(int t_progress);

private slots:
    void setDownloadProgress(long long t_bytesDownloaded, long long t_totalBytes);

private:
    bool runInternal();
    Action retryOrGoOffline(const QString& reason);
    bool tryStartOffline();

    void tryStartOfflineOrDisplayError(const QString& msg);

    Data resolveData();
    void update(
            const Locations& locations,
            const Data& data, const Api& api,
            QNetworkAccessManager& nam,
            CancellationToken cancellationToken);

    bool tryUpdate(
            const Locations& locations,
            const Data& data, const Api& api,
            QNetworkAccessManager& nam,
            CancellationToken cancellationToken);

    Data setupPatcherSecret(const Data& data, const Api& api, CancellationToken cancellationToken);

    std::unique_ptr<Data> m_runningData;
    ILauncherInterface& m_launcherInterface;
    CancellationTokenSource m_cancellationTokenSource;
    NetworkTest m_networkTest;
};
