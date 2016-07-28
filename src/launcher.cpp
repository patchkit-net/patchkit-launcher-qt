#include "launcher.h"
#include <qlogging.h>
#include <QtConcurrent/QtConcurrent>

Launcher::Launcher(ILauncherWindow *window, LauncherConfiguration configuration) :
    m_hasBeenStarted(false),
    m_hasBeenCancelled(false),
    m_hasFinished(false),
    m_window(window),
    m_configuration(configuration)
{
}

void Launcher::start()
{
    if(m_hasBeenStarted)
    {
        qFatal("launcher has been already started");
    }

    qDebug("launcher has been started");
    //QtConcurrent::run(&doWork);
}

void Launcher::cancel()
{
    m_hasBeenCancelled = true;
}

void Launcher::doWork()
{
    qDebug("launcher has been started");
    if(QFile::exists(m_configuration.launcherDataFileName))
    {

    }
}

bool Launcher::hasFinished()
{
    return m_hasFinished;
}

bool Launcher::hasBeenStarted()
{
    return m_hasBeenStarted;
}

bool Launcher::hasBeenCancelled()
{
    return m_hasBeenCancelled;
}
