/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include <QtNetwork>

#include "chunkeddownloader.h"

#include "customexceptions.h"
#include "contentsummary.h"
#include "downloader.h"
#include "logger.h"

#include "idownloadstrategy.h"

#include "config.h"

#include "downloaderoperator.h"

ChunkedDownloader::ChunkedDownloader(
        Downloader::TDataSource t_dataSource,
        const ContentSummary& t_contentSummary,
        HashFunc t_hashingStrategy,
        CancellationToken t_cancellationToken,
        LauncherState& t_state,
        const IApi& t_api
        )
    : m_hashingStrategy(t_hashingStrategy)
    , m_contentSummary(t_contentSummary)
    , m_cancellationToken(t_cancellationToken)
    , m_state(t_state)
    , m_dataSource(t_dataSource)
    , m_api(t_api)
{
}

QByteArray ChunkedDownloader::downloadFile(const QStringList& t_contentUrls)
{
    if (!m_hashingStrategy)
    {
        throw std::runtime_error("No hashing strategy specified.");
    }

    StringUrlProvider urlProvider(t_contentUrls);

    DownloaderOperator op(m_dataSource, urlProvider, m_cancellationToken);

    connect(&op, &DownloaderOperator::downloadProgress, this, &ChunkedDownloader::downloadProgress);

    ChunkedDownloadStrategy strategy(
                op,
                m_state,
                Config::minConnectionTimeoutMsec,
                Config::maxConnectionTimeoutMsec,
                *this);

    QByteArray data = op.download(strategy, m_cancellationToken);

    return data;
}

QVector<QByteArray> ChunkedDownloader::processChunks(QByteArray& t_data) const
{
    QVector<QByteArray> chunks;

    const int chunkSize = getChunkSize();
    for (int i = 0; i < t_data.size(); i += chunkSize)
    {
        QByteArray chunk = t_data.mid(i, chunkSize);

        chunks.push_back(chunk);
    }

    return chunks;
}

bool ChunkedDownloader::validateChunk(const QByteArray& t_chunkData, int t_chunkIndex) const
{
    if (t_chunkIndex > m_contentSummary.getChunksCount())
    {
        return false;
    }

    THash valid_hash = m_contentSummary.getChunkHash(t_chunkIndex);
    THash hash = m_hashingStrategy(t_chunkData);

    return hash == valid_hash;
}

QVector<bool> ChunkedDownloader::validateAllChunks(const QVector<QByteArray>& t_chunks) const
{
    QVector<bool> validChunkMap(t_chunks.size(), false);

    for (int i = 0; i < t_chunks.size(); i++)
    {
        validChunkMap[i] = validateChunk(t_chunks.at(i), i);
    }

    return validChunkMap;
}

int ChunkedDownloader::getChunkSize() const
{
    return m_contentSummary.getChunkSize();
}
