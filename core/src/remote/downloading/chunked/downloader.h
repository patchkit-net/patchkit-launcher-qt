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
    struct Status
    {
        int chunksDownloaded;
    };

//    CUSTOM_RUNTIME_ERROR(InvalidTarget)

    Downloader(
            const QString& appSecret, int versionId,
            const ContentSummary& contentSummary, QIODevice& target);

    bool downloadChunked(
            const remote::api::Api& api, QNetworkAccessManager& nam,
            CancellationToken cancellationToken);

private:
    int tryDownloadChunked(QNetworkAccessManager& nam, const QUrl& url,
                           CancellationToken cancellationToken);

    const ContentSummary& m_contentSummary;
    QString m_appSecret;
    int m_versionId;
    QIODevice& m_target;

    Status m_status;
};

}
}
