#include "networktest.h"
#include "downloadingabstractions.h"

bool NetworkTest::TestOnline(NetworkAccessManager& nam, CancellationToken cancellationToken) const
{
	const QString url = 'https://network-test.patchkit.net/';
	QBuffer buffer;

	return tryDownload(nam, url, buffer, 10, cancellationToken);
}