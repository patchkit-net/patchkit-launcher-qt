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
#include "staledownloadexception.h"

#include "config.h"

ChunkedDownloader::ChunkedDownloader(QNetworkAccessManager* t_dataSource, const ContentSummary& t_contentSummary, HashFunc t_hashingStrategy)
    : Downloader(t_dataSource)
    , m_contentSummary(t_contentSummary)
    , m_lastValidChunkIndex(0)
    , m_hashingStrategy(t_hashingStrategy)
    , m_running(true)
{
    connect(this, &ChunkedDownloader::downloadProgressChanged, this, &Downloader::downloadProgressChanged);
}

QByteArray ChunkedDownloader::downloadFile(const QString& t_urlPath, int t_requestTimeoutMsec, CancellationToken t_cancellationToken)
{
    if (!m_hashingStrategy)
    {
        throw std::runtime_error("No hashing strategy specified.");
    }

    QByteArray data;
    QNetworkRequest request;

    QUrl url(t_urlPath);

    request = QNetworkRequest(url);

    m_staleTimer.setInterval(Config::chunkedDownloadStaleTimeoutMsec);
    connect(&m_staleTimer, &QTimer::timeout, this, &ChunkedDownloader::staleTimerTimeout);
    m_staleTimer.start();

    m_running = true;

    bool downloadSuccesful = false;

    while (!shouldStop())
    {
        try
        {
            data = Downloader::downloadFile(request, t_requestTimeoutMsec, t_cancellationToken);

            if (validateReceivedData(data))
            {
                downloadSuccesful = true;
                break;
            }
        }
        catch (CancelledException&)
        {
            throw;
        }
        catch (TimeoutException&)
        {
            throw;
        }
        catch(...)
        {
            throw std::runtime_error("Unexpected exception in ChunkedDownloader");
        }
    }

    if (!downloadSuccesful)
        throw StaleDownloadException();

    for (QByteArray chunk : m_chunks)
    {
        data += chunk;
    }

    return data;
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
    m_staleTimer.start();
    TByteCount bytesInValidChunksDownloadedSoFar = m_lastValidChunkIndex * getChunkSize();
    emit downloadProgressChanged(bytesInValidChunksDownloadedSoFar + t_bytesDownloaded, bytesInValidChunksDownloadedSoFar+ t_totalBytes);
}

void ChunkedDownloader::staleTimerTimeout()
{
    abort();
}

bool ChunkedDownloader::shouldStop() const
{
    return !m_running;
}

void ChunkedDownloader::abort()
{
    emit terminate();

    m_running = false;
}

QVector<QByteArray> ChunkedDownloader::processChunks(QByteArray& t_data) const
{
    QVector<QByteArray> chunks;

    const int chunkSize = getChunkSize();
    for (int i = 0; i < t_data.size(); i += chunkSize)
    {
        // Get the chunk
        QByteArray chunk = t_data.mid(i, chunkSize);

        chunks.push_back(chunk);
    }

    return chunks;
}

bool ChunkedDownloader::validateReceivedData(QByteArray& t_data)
{
    QVector<QByteArray> chunks = processChunks(t_data);

    m_chunks += chunks;

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
