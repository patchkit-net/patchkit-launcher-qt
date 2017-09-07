#include "launcherstate.h"

LauncherState::LauncherState(QObject* parent)
    : QObject(parent)
    , m_state(State::Running)
    , m_response(Response::None)
{
}

LauncherState::State LauncherState::getState() const
{
    return m_state;
}

void LauncherState::setState(LauncherState::State t_state)
{
    m_state = t_state;
}

LauncherState::Response LauncherState::awaitResponseTo(DownloadError t_error, CancellationToken t_cancellationToken)
{
    QEventLoop loop;

    connect(&t_cancellationToken, &CancellationToken::cancelled, &loop, &QEventLoop::quit);
    connect(this, &LauncherState::respond, &loop, &QEventLoop::quit);

    emit error(t_error);

    loop.exec();

    t_cancellationToken.throwIfCancelled();

    return m_response;
}

void LauncherState::respondInternal(LauncherState::Response t_response)
{
    m_response = t_response;
}
