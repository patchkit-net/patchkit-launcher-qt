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

using remote::api::Api;

/* TODO:
 * The declaration of this class is relatively fine but it's underlying logic
 * is a sprawling mess of almost 300 lines.
 *
 * Splitting the implementation into separate source files would solve this problem.
 */
class LauncherWorker : public QThread
{
    Q_OBJECT

    void run() override;

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
    bool retryOrGoOffline(const QString& reason);

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
};
