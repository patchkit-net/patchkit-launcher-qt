#pragma once
#include <QNetworkAccessManager>
#include <QIODevice>

#include <cancellation/cancellationtoken.h>
#include <customexceptions.h>

#include <data/downloadrange.h>

namespace downloading
{
namespace abstractions
{

int getReplyStatusCode(const QNetworkReply* reply);

bool doesStatusCodeIndicateSuccess(int statusCode);

int waitUntilReplyIsReady(
        QNetworkReply* reply,
        int timeout,
        CancellationToken cancellationToken);

void bufferReply(
        QNetworkReply* reply,
        QIODevice& target,
        int timeout,
        CancellationToken cancellationToken);

bool tryDownload(
        QNetworkAccessManager& nam,
        QUrl url,
        QIODevice& target,
        int timeout,
        CancellationToken cancellationToken);

bool tryDownload(
        QNetworkAccessManager& nam,
        const QString& url,
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

bool tryRangedDownload(
        QNetworkAccessManager& nam,
        const QString& stringUrl,
        data::DownloadRange range,
        QIODevice& target,
        int timeout,
        CancellationToken cancellationToken);

} // namespace abstractions
} // namespace downloading
