#ifndef LAUNCHERWORKER_H
#define LAUNCHERWORKER_H

#include <QThread>

#include "launcherdata.h"
#include "launcherconfiguration.h"
#include "remotepatcher.h"

class LauncherThread : public QThread
{
    Q_OBJECT

    void run() Q_DECL_OVERRIDE;

public:
    LauncherThread(const LauncherConfiguration &configuration, RemotePatcher * const remotePatcher);

    void cancel();

signals:
    void bytesDownloadedChanged(const long& bytesDownloaded);
    void totalBytesChanged(const long& totalBytes);
    void statusChanged(const QString& status);
    void progressChanged(const int& progress);

private:
    LauncherData loadData();

    RemotePatcher * const m_remotePatcher;

    const LauncherConfiguration m_configuration;

    bool m_isCancelled;
};

#endif // LAUNCHERWORKER_H
