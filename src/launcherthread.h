/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include "launcherdata.h"
#include "remotepatcher.h"
#include "localpatcher.h"

class LauncherThread : public QThread
{
    Q_OBJECT

    void run() override;

public:
    LauncherThread();

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

    void runWithData(const LauncherData& t_data);

    void setupCurrentDirectory(const LauncherData& t_data) const;

    void updatePatcher(const LauncherData& t_data);
    void startPatcher(const LauncherData& t_data);

    RemotePatcher m_remotePatcher;
    LocalPatcher m_localPatcher;

    bool m_isCancelled;
    bool m_noError;

    const int downloadedProgressValue = 90;
};
