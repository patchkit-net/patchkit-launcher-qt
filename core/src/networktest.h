#pragma once

#include <QObject>
#include <QNetworkAccessManager>

#include "cancellation/cancellationtoken.h"

class NetworkTest
{
public:
    NetworkTest() : m_attempt(0) {}

    // Performs a simple conectivity test
    // Returns true if user is online
    bool isOnline(QNetworkAccessManager& nam, CancellationToken cancellationToken);

private:

    int m_attempt;

};
