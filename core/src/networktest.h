#pragma once

#include <QObject>

class NetworkTest : public QObject
{
    Q_OBJECT

public:
    // Performs a simple conectivity test
    // Returns true if user is online
    bool TestOnline(NetworkAccessManager& nam) const;

}