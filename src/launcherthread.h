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
    LauncherThread(const LauncherConfiguration& t_configuration, RemotePatcher * const t_remotePatcher);

    void cancel();

signals:
    void cancelled();
    void statusChanged(const QString& t_status);
    void progressChanged(int t_progress);

private slots:
    void downloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes);

private:
    LauncherData loadData() const;

    RemotePatcher * const m_remotePatcher;

    const LauncherConfiguration m_configuration;

    bool m_isCancelled;
};

#endif // LAUNCHERWORKER_H
