#pragma once

#include "data/data.h"
#include "data/contentsummary.h"
#include "remote/api/api.h"
#include "remote/downloading/downloadingabstractions.h"
#include "remote/downloading/chunked/chunkedbuffer.h"

#include <QNetworkAccessManager>
#include <QIODevice>
#include <QUrl>

namespace downloading
{
namespace chunked
{

class Downloader
{
public:
    Downloader(
            const QString& appSecret, int versionId,
            const ContentSummary& contentSummary, QIODevice& target);

    bool downloadChunked(
            const remote::api::Api& api, QNetworkAccessManager& nam,
            CancellationToken cancellationToken);

private:
    int tryDownloadChunked(QNetworkAccessManager& nam, ChunkedBuffer& chunkedBuffer, const QUrl& url,
                           CancellationToken cancellationToken);

    const ContentSummary& m_contentSummary;
    QString m_appSecret;
    int m_versionId;
    QIODevice& m_target;
};

}
}
