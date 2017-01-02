/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include <QtNetwork>

#include "chunkeddownloader.h"

#include "timeoutexception.h"
#include "contentsummary.h"
#include "downloader.h"
#include "logger.h"

ChunkedDownloader::ChunkedDownloader(const ContentSummary& t_contentSummary, HashFunc t_hashingStrategy)
    : m_contentSummary(t_contentSummary)
    , m_lastValidChunkIndex(0)
    , m_hashingStrategy(t_hashingStrategy)
{
    connect(this, &ChunkedDownloader::downloadProgressChanged, this, &Downloader::downloadProgressChanged);
}

void ChunkedDownloader::downloadFile(const QString& t_urlPath, const QString& t_filePath, int t_requestTimeoutMsec, CancellationToken t_cancellationToken)
{
    if (!m_hashingStrategy)
    {
        throw std::runtime_error("No hashing strategy specified.");
    }

    TSharedNetworkAccessManager accessManager;
    TSharedNetworkReply reply;
    QUrl url(t_urlPath);

    QNetworkRequest networkRequest = QNetworkRequest(url);

    Downloader::fetchReply(networkRequest, accessManager, reply);

    connect(accessManager.data(), &QNetworkAccessManager::networkAccessibleChanged, this, &ChunkedDownloader::watchNetorkAccessibility);

    connect(reply.data(), &QNetworkReply::downloadProgress, this, &ChunkedDownloader::downloadProgressChangedRelay);
    connect(this, &ChunkedDownloader::terminate, reply.data(), &QNetworkReply::abort);

    while (!shouldStop())
    {
        try
        {
            Downloader::waitForReply(reply, t_requestTimeoutMsec, t_cancellationToken);

            Downloader::waitForFileDownload(reply, t_cancellationToken);

            t_cancellationToken.throwIfCancelled();

            if (!validateReceivedData(reply))
            {
                restartDownload(reply, accessManager, url);
            }
            else
            {
                break;
            }
        }
        catch (TimeoutException&)
        {
            throw;
        }
    }

    QByteArray data;
    for (QByteArray chunk : m_chunks)
    {
        data += chunk;
    }

    Downloader::writeDownloadedData(data, t_filePath, t_cancellationToken);
}

void ChunkedDownloader::watchNetorkAccessibility(QNetworkAccessManager::NetworkAccessibility t_accessible)
{
    if (t_accessible == QNetworkAccessManager::NetworkAccessibility::NotAccessible)
    {
        logDebug("Network not accessible");
        emit terminate();
    }
    else if (t_accessible == QNetworkAccessManager::NetworkAccessibility::Accessible)
    {
        logDebug("Network accessible.");
    }
    else
    {
        logDebug("Undetermined network status.");
    }
}

void ChunkedDownloader::downloadProgressChangedRelay(const Downloader::TByteCount& t_bytesDownloaded, const Downloader::TByteCount& t_totalBytes)
{
    TByteCount bytesInValidChunksDownloadedSoFar = m_lastValidChunkIndex * getChunkSize();
    emit downloadProgressChanged(bytesInValidChunksDownloadedSoFar + t_bytesDownloaded, bytesInValidChunksDownloadedSoFar+ t_totalBytes);
}

bool ChunkedDownloader::shouldStop() const
{
    /* TODO Implement this method
     * - the program works without it, but will basically run forever
     */

    // Return true if the maximum time is reached (2 min?)

    // Otherwise return false
    return false;
}

QVector<QByteArray> ChunkedDownloader::processChunks(TSharedNetworkReplyRef t_reply) const
{
    QByteArray data = t_reply->readAll();
    QVector<QByteArray> chunks;

    const int chunkSize = getChunkSize();
    for (int i = 0; i < data.size(); i += chunkSize)
    {
        // Get the chunk
        QByteArray chunk = data.mid(i, chunkSize);

        chunks.push_back(chunk);
    }

    return chunks;
}

bool ChunkedDownloader::validateReceivedData(TSharedNetworkReplyRef t_reply)
{
    QVector<QByteArray> chunks = processChunks(t_reply);

    m_chunks += chunks;

    //m_lastValidChunkIndex = m_chunks.size() - 1;

    for (int i = m_lastValidChunkIndex; i < m_chunks.size(); i++)
    {
        // Get the content summary hash
        THash valid_hash = m_contentSummary.getChunkHash(i);

        // At this point the m_hashingStrategy is guaranteed to be valid, no need to check if it's null.
        THash hash = m_hashingStrategy(m_chunks.at(i));

        if (valid_hash != hash)
        {
            // Mark the previous index as the last valid
            m_lastValidChunkIndex = i - 1;

            if (m_lastValidChunkIndex < 0)
                m_lastValidChunkIndex = 0;

            // Rend the remaining invalid chunks
            m_chunks.remove(i, m_chunks.size() - i);

            // Signal to restart download
            return false;
        }
        else
        {
            m_lastValidChunkIndex = i;
        }
    }

    return m_chunks.size() == m_contentSummary.getChunksCount();
}

const int ChunkedDownloader::getChunkSize() const
{
    return m_contentSummary.getChunkSize();
}

void ChunkedDownloader::restartDownload(TSharedNetworkReplyRef t_reply, TSharedNetworkAccessManagerRef t_networkManager, const QUrl& t_url) const
{
    disconnect(t_reply.data(), &QNetworkReply::downloadProgress, this, &ChunkedDownloader::downloadProgressChangedRelay);
    disconnect(this, &ChunkedDownloader::terminate, t_reply.data(), &QNetworkReply::abort);

    // Reformulate the request
    QByteArray header = "bytes=" + QByteArray::number((m_lastValidChunkIndex + 1) * getChunkSize()) + "-";
    QNetworkRequest request (t_url);
    request.setRawHeader("Range", header);

    // Fetch a new reply
    Downloader::fetchReply(request, t_networkManager, t_reply);

    connect(t_reply.data(), &QNetworkReply::downloadProgress, this, &ChunkedDownloader::downloadProgressChangedRelay);
    connect(this, &ChunkedDownloader::terminate, t_reply.data(), &QNetworkReply::abort);

}
