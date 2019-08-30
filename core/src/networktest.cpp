#include "networktest.h"
#include "remote/downloading/downloadingabstractions.h"

#include <QBuffer>

bool NetworkTest::isOnline(QNetworkAccessManager& nam, CancellationToken cancellationToken)
{
    const QString url = "https://network-test.patchkit.net/";
    QBuffer buffer;

    m_attempt += 1;

    int timeout = 10000;
    if (m_attempt == 2)
    {
        timeout = 30000;
    }
    else if (m_attempt >= 3)
    {
        timeout = 60000;
    }

    return downloading::abstractions::tryDownload(nam, url, buffer, timeout, cancellationToken);
}
