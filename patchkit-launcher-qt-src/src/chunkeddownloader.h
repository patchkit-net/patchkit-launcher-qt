/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>
#include <QVector>
#include <QStack>

#include "chunkeddownloadstrategy.h"
#include "downloader.h"
#include "iapi.h"
#include "hashingstrategy.h"

class ContentSummary;

class ChunkedDownloader : public QObject
{
    Q_OBJECT

public:
    ChunkedDownloader(
            Downloader::TDataSource t_dataSource,
            const ContentSummary& t_contentSummary,
            HashFunc t_hashingStrategy,
            CancellationToken t_cancellationToken,
            const IApi& t_api
            );

    QByteArray downloadFile(const QStringList& t_contentUrls);

    QVector<QByteArray> processChunks(QByteArray& t_data) const;
    QVector<bool>       validateAllChunks(const QVector<QByteArray>& t_chunks) const;
    bool                validateChunk(const QByteArray& t_chunkData, int t_chunkIndex) const;

signals:
    void downloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes);

private:
    HashFunc                m_hashingStrategy;
    const ContentSummary&   m_contentSummary;
    CancellationToken       m_cancellationToken;
    Downloader::TDataSource m_dataSource;

    const IApi&             m_api;

    int   getChunkSize() const;

    friend class ChunkedDownloadStrategy;
};
