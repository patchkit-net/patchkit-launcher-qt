#pragma once

#include <QObject>
#include <QNetworkAccessManager>

#include "cancellation/cancellationtoken.h"

class NetworkTest
{
public:
    // Performs a simple conectivity test
    // Returns true if user is online
    bool isOnline(QNetworkAccessManager& nam, CancellationToken cancellationToken) const;

};
