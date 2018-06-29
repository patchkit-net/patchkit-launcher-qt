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
#include "utilities.h"

struct ProgressData
{
    QString status;
    int progress;
};

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

    LauncherWorker(QObject* parent = nullptr);

    void cancel();

    Result result() const;
signals:
    void statusChanged(const QString& t_status);
    void progressChanged(int t_progress);

    void query(const QString& t_title, const QString& t_message);

    void queryAnswer(bool answer);

public slots:
    void stop();

private slots:
    void setDownloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes);

private:
    Data resolveData() const;

    void runWithData(const Data& t_data);

    Data setupPatcherSecret(const Data& t_data);

    Utilities::Maybe<QString> tryToFetchPatcherSecret(const Data& t_data);

    void updatePatcher(const Data& t_data);
    void startPatcher(const Data& t_data);

    bool ask(const QString& t_title, const QString& t_message);

    CancellationTokenSource m_cancellationTokenSource;

    QNetworkAccessManager m_networkAccessManager;

    Api m_api;
    RemotePatcherData m_remotePatcher;
    LocalPatcherData m_localPatcher;

    Result m_result;
};
