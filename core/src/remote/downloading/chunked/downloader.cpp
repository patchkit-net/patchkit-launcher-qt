#include "downloader.h"
#include "config.h"
#include "remote/downloading/progressdevice.h"

downloading::chunked::Downloader::Downloader(
        const QString& appSecret, int versionId,
        const ContentSummary& contentSummary, QIODevice& target)
    : m_contentSummary(contentSummary)
    , m_appSecret(QString(appSecret))
    , m_versionId(versionId)
    , m_target(target)
{

}

bool downloading::chunked::Downloader::downloadChunked(
        const remote::api::Api& api, QNetworkAccessManager& nam,
        CancellationToken cancellationToken)
{
    QStringList contentUrls = api.getContentUrls(this->m_appSecret, m_versionId, cancellationToken);

    auto expectedHashes = QVector<THash>();
    for (int i = 0; i < m_contentSummary.getChunksCount(); i++)
    {
        expectedHashes.push_back(m_contentSummary.getChunkHash(i));
    }

    ChunkedBuffer chunkedBuffer(
                expectedHashes, m_contentSummary.getChunkSize(), HashingStrategy::xxHash, m_target);
    chunkedBuffer.open(QIODevice::WriteOnly);

    for (int i = 0; i < Config::maxInvalidChunksCount; i++)
    {
        for (QString url : contentUrls)
        {
            auto validChunksWritten = tryDownloadChunked(nam, chunkedBuffer, url, cancellationToken);

            if (validChunksWritten == m_contentSummary.getChunksCount())
            {
                qInfo() << "Succesfully downloaded " << validChunksWritten << " out of " << m_contentSummary.getChunksCount() << " chunks";
                return true;
            }

            qInfo() << "Written " << validChunksWritten << " chunks out of " << m_contentSummary.getChunksCount();
        }
    }

    qWarning() << "Chunked download did not succeed";
    return false;
}

int downloading::chunked::Downloader::tryDownloadChunked(
        QNetworkAccessManager& nam, ChunkedBuffer& chunkedBuffer, const QUrl& url, CancellationToken cancellationToken)
{
    qInfo() << "Downloading chunked file from " << url;

    data::DownloadRange range(chunkedBuffer.validChunksWritten() * m_contentSummary.getChunkSize());

    try
    {
        qInfo() << "Downloading from " << url << " with range " << range.start << "-" << range.end;
        downloading::abstractions::tryRangedDownload(
            nam, url, range, chunkedBuffer, Config::downloadTimeoutMsec, cancellationToken);
    }
    catch (ChunkedBuffer::ChunkVerificationException& e)
    {
        qWarning() << "Invalid chunk: " << e.what();
    }

    return chunkedBuffer.validChunksWritten();
}
