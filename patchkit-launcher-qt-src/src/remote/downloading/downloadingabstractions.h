#pragma once
#include <QNetworkAccessManager>
#include <QIODevice>

#include "cancellation/cancellationtoken.h"
#include "data/downloadrange.h"

namespace downloading
{
namespace abstractions
{
int getReplyStatusCode(const QNetworkReply* reply);

bool doesStatusCodeIndicateSuccess(int statusCode);

bool tryDownload(
        QNetworkAccessManager& nam,
        QUrl url,
        QIODevice& target,
        int timeout,
        CancellationToken cancellationToken);

bool tryDownload(
        QNetworkAccessManager& nam,
        QString url,
        QIODevice& target,
        int timeout,
        CancellationToken cancellationToken);

bool tryRangedDownload(
        QNetworkAccessManager& nam,
        QUrl url,
        data::DownloadRange range,
        QIODevice& target,
        int timeout,
        CancellationToken cancellationToken);

} // namespace abstractions
} // namespace downloading
