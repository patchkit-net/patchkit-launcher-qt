#include "networktest.h"
#include "remote/downloading/downloadingabstractions.h"
#include "remote/downloading/timeout.h"

#include <QBuffer>

bool NetworkTest::isOnline(QNetworkAccessManager& nam, CancellationToken cancellationToken) const
{
    const QString url = "https://network-test.patchkit.net/";
    QBuffer buffer;

    return downloading::abstractions::tryDownload(nam, url, buffer, Timeout::seconds(10), cancellationToken);
}
