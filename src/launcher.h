#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "ilauncherwindow.h"
#include "launcherconfiguration.h"

class Launcher
{
public:
    explicit Launcher(ILauncherWindow *launcherWindow, LauncherConfiguration launcherConfiguration);

    void start();
    void cancel();

    bool hasFinished();
    bool hasBeenStarted();
    bool hasBeenCancelled();
private:
    void doWork();

    ILauncherWindow *m_window;
    LauncherConfiguration m_configuration;

    bool m_hasFinished;
    bool m_hasBeenStarted;
    bool m_hasBeenCancelled;
};

#endif // LAUNCHER_H
