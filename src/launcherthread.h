/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#ifndef LAUNCHERTHREAD_H
#define LAUNCHERTHREAD_H

#include <QThread>
#include <memory>

#include "launcherdata.h"
#include "launcherconfiguration.h"
#include "remotepatcher.h"
#include "localpatcher.h"

class LauncherThread : public QThread
{
    Q_OBJECT

    void run() override;

public:
    // Note that t_remotePatcher and t_localPatcher are automaticaly moved to this thread - their usage outside of this thread is forbidden.
    LauncherThread(const LauncherConfiguration& t_configuration,
                   std::shared_ptr<RemotePatcher> t_remotePatcher,
                   std::shared_ptr<LocalPatcher> t_localPatcher);

    void cancel();

    bool isSuccess();
signals:
    void statusChanged(const QString& t_status);
    void progressChanged(int t_progress);

private slots:
    void setDownloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes);

private:
    std::shared_ptr<RemotePatcher> m_remotePatcher;
    std::shared_ptr<LocalPatcher> m_localPatcher;

    const LauncherConfiguration m_configuration;

    bool m_isCancelled;
    bool m_isSuccess;

    const int downloadedProgressValue = 90;
};

#endif // LAUNCHERTHREAD_H
