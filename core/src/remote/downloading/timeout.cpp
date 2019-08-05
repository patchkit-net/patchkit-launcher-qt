#include "timeout.h"

Timeout Timeout::seconds(int seconds)
{
    return Timeout(seconds * 1000);
}

Timeout Timeout::milliseconds(int msec)
{
    return Timeout(msec);
}

int Timeout::getMilliseconds() const
{
    return m_milliseconds;
}

Timeout::Timeout(int msec)
    : m_milliseconds(msec)
{
}
