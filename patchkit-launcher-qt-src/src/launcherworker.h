/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include "data.h"
#include "remotepatcherdata.h"
#include "localpatcherdata.h"
#include "cancellationtokensource.h"
#include "api.h"
#include "lockfile.h"
#include "data/networkstatus.hpp"

class LauncherWorker : public QThread
{
    Q_OBJECT

    void run() override;

public:
    enum Result
    {
        NONE,
        CANCELLED,
        SUCCESS,
        FAILED,
        FATAL_ERROR,
        LOCKED,
        CONNECTION_ERROR
    };

    LauncherWorker(LauncherState& t_launcherState, QObject* parent = nullptr);

    void cancel();

    bool canStartPatcher() const;
    void startPatcher(LauncherCore::Types::NetworkStatus networkStatus);

    void resolveData();

    Result result() const;
signals:
    void statusChanged(const QString& t_status);
    void progressChanged(int t_progress);

public slots:
    void stop();

private slots:
    void setDownloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes);

private:
    void runWithData(Data& t_data);

    void setupPatcherSecret(Data& t_data);

    bool tryToFetchPatcherSecret(Data& t_data);

    bool isLocalPatcherInstalled() const;
    void updatePatcher(const Data& t_data);
    void startPatcher(const Data& t_data, LauncherCore::Types::NetworkStatus networkStatus);

    CancellationTokenSource m_cancellationTokenSource;
    LauncherState& m_launcherState;

    QNetworkAccessManager m_networkAccessManager;

    Api m_api;
    RemotePatcherData m_remotePatcher;
    LocalPatcherData m_localPatcher;

    Result m_result;

    Data m_data;
};
