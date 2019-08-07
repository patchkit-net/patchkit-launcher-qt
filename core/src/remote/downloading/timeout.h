#pragma once

#include <QString>

class Timeout
{
public:
    Timeout(const Timeout&) = default;

    static Timeout seconds(int seconds);
    static Timeout milliseconds(int msec);

    int getMilliseconds() const;

private:
    Timeout(int msec);
    int m_milliseconds;
};
