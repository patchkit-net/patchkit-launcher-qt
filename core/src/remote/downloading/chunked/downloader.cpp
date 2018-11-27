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
    if (ChunkedBuffer::verifyTarget(m_target, m_contentSummary.getChunkSize()))
    {
        throw InvalidTarget("Target contains invalid data");
    }
}

bool downloading::chunked::Downloader::downloadChunked(
        const Api& api, QNetworkAccessManager& nam,
        CancellationToken cancellationToken)
{
    ChunkInfo chunkInfo(m_contentSummary);
    ChunkedBuffer chunkedBuffer(chunkInfo, HashingStrategy::xxHash, m_target);
    ProgressDevice progressDevice(chunkedBuffer, m_target.size());

    QObject::connect(&progressDevice, &ProgressDevice::onProgress, this, &Downloader::progressRelay);

    QStringList contentUrls = api.getContentUrls(m_appSecret, m_versionId, cancellationToken);

    for (QString url : contentUrls)
    {
        if (abstractions::tryDownload(
                    nam, url, progressDevice, Config::downloadTimeoutMsec, cancellationToken))
        {
            return true;
        }
    }

    return false;
}

void downloading::chunked::Downloader::progressRelay(qint64 bytes)
{
    auto total = m_contentSummary.getChunkSize() * m_contentSummary.getChunksCount();
    emit onProgress(bytes, total);
}
