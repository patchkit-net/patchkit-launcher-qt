#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QObject>

#include "launcherconfiguration.h"
#include "launcherthread.h"
#include "remotepatcher.h"

class Launcher : public QObject
{
    Q_OBJECT

public:
    Launcher(const LauncherConfiguration& t_configuration, RemotePatcher * const t_remotePatcher);
    ~Launcher();

    void start();
    void cancel() const;

signals:
    void finished();
    void statusChanged(const QString& t_status);
    void progressChanged(int t_progress);

private:
    LauncherThread *m_thread;

    RemotePatcher * const m_remotePatcher;

    const LauncherConfiguration m_configuration;
};

#endif // LAUNCHER_H
