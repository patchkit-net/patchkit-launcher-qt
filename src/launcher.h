#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QObject>

#include "launcherdata.h"
#include "launcherconfiguration.h"
#include "launcherthread.h"
#include "remotepatcher.h"

class Launcher : public QObject
{
    Q_OBJECT

public:
    Launcher(const LauncherConfiguration &configuration, RemotePatcher * const remotePatcher);
    ~Launcher();

    void start();
    void cancel();

signals:
    void finished();
    void bytesDownloadedChanged(const long& bytesDownloaded);
    void totalBytesChanged(const long& totalBytes);
    void statusChanged(const QString& status);
    void progressChanged(const int& progress);

private:
    LauncherThread *m_thread;

    RemotePatcher * const m_remotePatcher;

    const LauncherConfiguration m_configuration;
};

#endif // LAUNCHER_H
