#include "launcher.h"
#include "launcherexception.h"
#include <QDebug>

Launcher::Launcher(const LauncherConfiguration &configuration, RemotePatcher * const remotePatcher) :
    m_configuration(configuration),
    m_remotePatcher(remotePatcher),
    m_thread(nullptr)
{
}

Launcher::~Launcher()
{
    if(m_thread != nullptr)
    {
        m_thread->cancel();
        m_thread->wait();
        delete m_thread;
    }
}

void Launcher::start()
{
    if(m_thread != nullptr)
    {
        throw LauncherException("Launcher has already started.");
    }

    qDebug("Starting launcher thread.");

    m_thread = new LauncherThread(m_configuration, m_remotePatcher);

    connect(m_thread, &LauncherThread::finished, this, &Launcher::finished);
    connect(m_thread, &LauncherThread::bytesDownloadedChanged, this, &Launcher::bytesDownloadedChanged);
    connect(m_thread, &LauncherThread::totalBytesChanged, this, &Launcher::totalBytesChanged);
    connect(m_thread, &LauncherThread::statusChanged, this, &Launcher::statusChanged);
    connect(m_thread, &LauncherThread::progressChanged, this, &Launcher::progressChanged);

    m_thread->start();
}

void Launcher::cancel()
{
    if(m_thread != nullptr)
    {
        m_thread->cancel();
    }
}
