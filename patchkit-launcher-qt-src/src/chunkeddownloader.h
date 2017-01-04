/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#ifndef CHUNKEDDOWNLOADER_H
#define CHUNKEDDOWNLOADER_H

#include <QObject>
#include <QVector>
#include <QStack>

#include "downloader.h"

#include "hashingstrategy.h"

class ContentSummary;

class QNetworkReply;
class QNetworkRequest;
class QNetworkAccessManager;

/**
 * @brief
 * Downloads files in chunks which are specified by the Content Summary.
 *
 * @details
 * As it is implemented now, the Chunked Downloader downloads the data through the QNetworkReply.
 * When the connection is broken or for any other reason the download has stopped, it validates all the downloaded data.
 * Once the data is validated the ChunkedDownloader proceeds to restart the download from the first invalid chunk (or from the start if no chunks were received)
 *
 * @note
 * This means that if in a set of 20 chunks the chunk #5 happened to be invalid but all the other ones were valid
 * the download will resume from the 5th chunk.
 */
class ChunkedDownloader : public Downloader
{
    Q_OBJECT

public:
    ChunkedDownloader(const ContentSummary& t_contentSummary, HashFunc t_hashingStrategy);

    void downloadFile(const QString& t_urlPath, const QString& t_filePath, int t_requestTimeoutMsec, CancellationToken t_cancellationToken) override;

signals:
    void downloadProgressChanged(const TByteCount& t_bytesDownloaded, const TByteCount& t_totalBytes);
    void terminate();

private slots:
    void watchNetorkAccessibility(QNetworkAccessManager::NetworkAccessibility t_accessible);
    void downloadProgressChangedRelay(const TByteCount& t_bytesDownloaded, const TByteCount& t_totalBytes);
    void staleTimerTimeout();

public slots:
    void abort();

private:

    QTimer                  m_staleTimer;

    bool                    m_running;

    QVector<QByteArray>     m_chunks;
    int                     m_lastValidChunkIndex;
    HashFunc                m_hashingStrategy;
    const ContentSummary&   m_contentSummary;


    QVector<QByteArray> processChunks(TSharedNetworkReply& t_reply) const;

    bool        validateReceivedData(TSharedNetworkReply& t_reply);
    void        restartDownload(TSharedNetworkReply& t_reply, TSharedNetworkAccessManager& t_networkManager, const QUrl& t_url) const;
    const int   getChunkSize() const;

    bool        shouldStop() const;
};

#endif // CHUNKEDDOWNLOADER_H