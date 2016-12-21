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

class ContentSummary;

class QNetworkReply;
class QNetworkRequest;
class QNetworkAccessManager;

class ChunkedDownloader : public Downloader
{
    Q_OBJECT

    const int MAX_DOWNLOAD_QUEUE_SIZE = 15;

    typedef long long TByteCount;

    typedef QString (*HashingStrategy)(QByteArray bytes);

public:
    ChunkedDownloader(const ContentSummary& t_contentSummary, HashingStrategy t_hashingStrategy);

    //virtual void downloadFile(const QString& t_urlPath, const QString& t_filePath, int t_requestTimeoutMsec, CancellationToken t_cancellationToken) const override;
    void downloadFile(const QString& t_urlPath, const QString& t_filePath, int t_requestTimeoutMsec, CancellationToken t_cancellationToken);

signals:
    void downloadProgressChanged(const TByteCount& t_bytesDownloaded, const TByteCount& t_totalBytes);

private:

    // Private variables

    HashingStrategy m_hashingStrategy;

    int m_lastValidChunkIndex;

    const ContentSummary& m_contentSummary;

    int m_maxNumberOfAttepmts;

    QVector<QByteArray> m_chunks;

    // Private methods

    void restartDownload(TSharedNetworkReplyRef t_reply, TSharedNetworkAccessManagerRef t_networkManager, const QUrl& t_url);

    QVector<QByteArray> processChunks(TSharedNetworkReplyRef t_reply);
    bool validateReceivedData(TSharedNetworkReplyRef t_reply);

    bool shouldStop() const;

    void writeDownloadedFile(const QString& t_filePath) const;

    const int& getChunkSize() const;
};

#endif // CHUNKEDDOWNLOADER_H
