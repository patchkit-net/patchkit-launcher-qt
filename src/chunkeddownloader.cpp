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

ChunkedDownloader::ChunkedDownloader(const ContentSummary& t_contentSummary, HashingStrategy t_hashingStrategy)
    : m_contentSummary(t_contentSummary)
    , m_lastValidChunkIndex(0)
    , m_hashingStrategy(t_hashingStrategy)
{
}

//void ChunkedDownloader::downloadFile(const QString& t_urlPath, const QString& t_filePath, int t_requestTimeoutMsec, CancellationToken t_cancellationToken) const
//{

//}

void ChunkedDownloader::downloadFile(const QString & t_urlPath, const QString & t_filePath, int t_requestTimeoutMsec, CancellationToken t_cancellationToken)
{
    TSharedNetworkAccessManager accessManager;
    TSharedNetworkReply reply;
    QUrl url(t_urlPath);

    // Formulate the request
    QNetworkRequest networkRequest = QNetworkRequest(url);

    Downloader::fetchReply(networkRequest, accessManager, reply);

    connect(reply.data(), &QNetworkReply::downloadProgress, this, &ChunkedDownloader::downloadProgressChanged);

    while (!shouldStop())
    {
        try
        {
            waitForReply(reply, t_requestTimeoutMsec, t_cancellationToken);

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

        }
    }

    writeDownloadedFile(t_filePath);
}

bool ChunkedDownloader::shouldStop() const
{
    // Return true if the maximum time is reached (2 min?)

    // Otherwise return false
    return false;
}

QVector<QByteArray> ChunkedDownloader::processChunks(TSharedNetworkReplyRef t_reply)
{
    QByteArray data = t_reply->readAll();
    QVector<QByteArray> chunks;

    for (int i = 0; i < data.size(); i += getChunkSize())
    {
        // Get the chunk
        QByteArray chunk = data.mid(i, getChunkSize());

        chunks.push_back(chunk);
    }

    return chunks;
}

bool ChunkedDownloader::validateReceivedData(TSharedNetworkReplyRef t_reply)
{
    QVector<QByteArray> chunks = processChunks(t_reply);

    m_chunks += chunks;

    m_lastValidChunkIndex = m_chunks.size() - 1;

    for (int i = 0; i < m_chunks.size(); i++)
    {
        // Get the content summary hash
        THash valid_hash = m_contentSummary.getChunkHash(i);

        // Hash
        if (!m_hashingStrategy)
        {
            // !!!!! No hashing strategy, should throw an exception, for now only signals to go ahead with reassembling the data !!!!
            return m_chunks.size() == m_contentSummary.getChunksCount();
        }

        THash hash = m_hashingStrategy(m_chunks.at(i), m_contentSummary);

        if (valid_hash != hash)
        {
            // Mark the previous index as the last valid
            m_lastValidChunkIndex = i - 1;

            // Rend the remaining chunks
            m_chunks.remove(i, m_chunks.size() - i);

            // Signal to restart download
            return false;
        }
    }

    return m_chunks.size() == m_contentSummary.getChunksCount();
}

const int& ChunkedDownloader::getChunkSize() const
{
    return m_contentSummary.getChunkSize();
}

void ChunkedDownloader::writeDownloadedFile(const QString& t_filePath) const
{
    logInfo("Writing downloaded data to file - %1", .arg(t_filePath));

    QFile file(t_filePath);

    if (!file.open(QFile::WriteOnly))
    {
        throw std::runtime_error("Couldn't open file for download.");
    }

    QByteArray data;
    for (QByteArray chunk : m_chunks)
    {
        data.append(chunk);
    }

    file.write(data);

    file.close();
}

void ChunkedDownloader::restartDownload(TSharedNetworkReplyRef t_reply, TSharedNetworkAccessManagerRef t_networkManager, const QUrl& t_url)
{
    disconnect(t_reply.data(), &QNetworkReply::downloadProgress, this, &ChunkedDownloader::downloadProgressChanged);

    // Reformulate the request

    QByteArray header = "bytes=" + QByteArray::number((m_lastValidChunkIndex + 1) * getChunkSize()) + "-";

    QNetworkRequest request (t_url);
    request.setRawHeader("Range", header);

    Downloader::fetchReply(request, t_networkManager, t_reply);

    connect(t_reply.data(), &QNetworkReply::downloadProgress, this, &ChunkedDownloader::downloadProgressChanged);
}
