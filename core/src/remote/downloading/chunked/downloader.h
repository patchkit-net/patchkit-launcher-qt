#pragma once

#include "data/data.h"
#include "data/contentsummary.h"
#include "remote/api/api.h"
#include "remote/downloading/downloadingabstractions.h"
#include "remote/downloading/chunked/chunkedbuffer.h"
#include "customexceptions.h"

#include <QNetworkAccessManager>
#include <QIODevice>

namespace downloading
{
namespace chunked
{

class Downloader
{
public:
    CUSTOM_RUNTIME_ERROR(InvalidTarget)

    Downloader(
            const QString& appSecret, int versionId,
            const ContentSummary& contentSummary, QIODevice& target);

    bool downloadChunked(
            const Api& api, QNetworkAccessManager& nam,
            CancellationToken cancellationToken);

private:
    int tryDownloadChunked(QNetworkAccessManager& nam, ChunkedBuffer& chunkedBuffer, const QUrl& url,
                           CancellationToken cancellationToken);

    const QString& m_appSecret;
    int m_versionId;

    const ContentSummary& m_contentSummary;
    QIODevice& m_target;
};

}
}
