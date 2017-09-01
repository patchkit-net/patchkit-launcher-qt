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
