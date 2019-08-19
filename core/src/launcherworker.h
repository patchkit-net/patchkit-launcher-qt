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
#include "locations/launcher.h"
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
    void setDisplayName(const QString displayName);

private slots:
    void setDownloadProgress(long long t_bytesDownloaded, long long t_totalBytes);

private:
    bool runInternal();
    Action retryOrGoOffline(const QString& reason);
    bool tryStartOffline();

    void tryStartOfflineOrDisplayError(const QString& msg);
    void trySetDisplayName(const Api& api, const Secret& applicationSecret, CancellationToken cancellationToken);

    static Data resolveData();

    void update(
            const locations::Launcher& locations,
            const Secret& appSecret, const Secret& patcherSecret, const Api& api,
            QNetworkAccessManager& nam,
            CancellationToken cancellationToken);

    bool tryUpdate(
            const locations::Launcher& locations,
            const Secret& appSecret, const Secret& patcherSecret, const Api& api,
            QNetworkAccessManager& nam,
            CancellationToken cancellationToken);

    std::unique_ptr<locations::Launcher> tryLoadLocations();

    Secret setupPatcherSecret(const Secret& appSecret, const Secret& patcherSecret, const Api& api, CancellationToken cancellationToken);

    std::unique_ptr<Data> m_runningData;
    ILauncherInterface& m_launcherInterface;
    CancellationTokenSource m_cancellationTokenSource;
    NetworkTest m_networkTest;
};
