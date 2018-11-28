#include "downloader.h"
#include "config.h"
#include "remote/downloading/progressdevice.h"

downloading::chunked::Downloader::Downloader(
        const QString& appSecret, int versionId,
        const ContentSummary& contentSummary, QIODevice& target)
    : m_appSecret(appSecret)
    , m_versionId(versionId)
    , m_contentSummary(contentSummary)
    , m_target(target)
{
    m_status.chunksDownloaded = 0;
}

bool downloading::chunked::Downloader::downloadChunked(
        const Api& api, QNetworkAccessManager& nam,
        CancellationToken cancellationToken)
{
    QStringList contentUrls = api.getContentUrls(m_appSecret, m_versionId, cancellationToken);

    for (int i = 0; i < Config::maxInvalidChunksCount; i++)
    {
        for (QString url : contentUrls)
        {
            int chunksDownloaded = tryDownloadChunked(nam, url, cancellationToken);

            if (chunksDownloaded == m_contentSummary.getChunksCount())
            {
                return true;
            }

            m_status.chunksDownloaded += chunksDownloaded;
        }
    }

    return false;
}

int downloading::chunked::Downloader::tryDownloadChunked(
        QNetworkAccessManager& nam, const QUrl& url, CancellationToken cancellationToken)
{
    auto expectedHashes = QVector<THash>();

    for (int i = m_status.chunksDownloaded; i < m_contentSummary.getChunksCount(); i++)
    {
        expectedHashes.push_back(m_contentSummary.getChunkHash(i));
    }

    ChunkedBuffer chunkedBuffer(expectedHashes, m_contentSummary.getChunkSize(), HashingStrategy::xxHash, m_target);
    chunkedBuffer.open(QIODevice::WriteOnly);

    data::DownloadRange range(m_status.chunksDownloaded * m_contentSummary.getChunkSize());

    try
    {
        if (downloading::abstractions::tryRangedDownload(
                    nam, url, range, chunkedBuffer, Config::downloadTimeoutMsec, cancellationToken))
        {
            chunkedBuffer.flush();
        }
    }
    catch (ChunkedBuffer::ChunkVerificationException)
    {
        qWarning() << "Invalid chunk";
        return chunkedBuffer.validChunksWritten();
    }

    return chunkedBuffer.validChunksWritten();
}
