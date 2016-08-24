/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include "data.h"
#include "remotepatcherdata.h"
#include "localpatcherdata.h"
#include "cancellationtokensource.h"

class LauncherWorker : public QThread
{
    Q_OBJECT

    void run() override;

public:
    LauncherWorker();

    void cancel();

    bool noError() const;
signals:
    void statusChanged(const QString& t_status);
    void progressChanged(int t_progress);

private slots:
    void setDownloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes);

private:
#ifdef Q_OS_WIN
    void runWithDataFromResource();
#endif
    void runWithDataFromFile();

    void runWithData(const Data& t_data);

    void setupCurrentDirectory(const Data& t_data) const;

    void updatePatcher(const Data& t_data);
    void startPatcher(const Data& t_data);

    RemotePatcherData m_remotePatcher;
    LocalPatcherData m_localPatcher;

    std::shared_ptr<CancellationTokenSource> m_cancellationTokenSource;
    bool m_noError;
};
