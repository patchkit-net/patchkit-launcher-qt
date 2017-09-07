/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcherstate.h"

LauncherState::LauncherState(QObject* parent)
    : QObject(parent)
    , m_state(State::Running)
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

void LauncherState::awaitResponseTo(DownloadError t_error, CancellationToken t_cancellationToken)
{
    QEventLoop loop;

    connect(&t_cancellationToken, &CancellationToken::cancelled, &loop, &QEventLoop::quit);
    connect(this, &LauncherState::respond, &loop, &QEventLoop::quit);

    emit error(t_error);

    loop.exec();

    t_cancellationToken.throwIfCancelled();
}
