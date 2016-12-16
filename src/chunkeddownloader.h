/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#ifndef CHUNKEDDOWNLOADER_H
#define CHUNKEDDOWNLOADER_H

#include <QObject>
#include <QVector>
#include <QStack>

#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>

#include "cancellationtoken.h"

class ContentSummary;

class QNetworkReply;
class QNetworkRequest;
class QNetworkAccessManager;

class ChunkedDownloader : public QObject
{
    Q_OBJECT

    const int MAX_DOWNLOAD_QUEUE_SIZE = 15;

    typedef long long TByteCount;

public:
    ChunkedDownloader(const ContentSummary& t_contentSummary, CancellationToken t_cancellationToken);
    ~ChunkedDownloader();

    void downloadFile(const QString& t_urlPath, const QString& t_filePath, int t_requestTimeoutMsec);

signals:
    void downloadProgressChanged(const TByteCount& t_bytesDownloaded, const TByteCount& t_totalBytes);

private slots:
    void watchDownloadProgress(const TByteCount& t_bytesDownloaded, const TByteCount& t_totalBytes);

    void onNetworkStatusChanged(QNetworkAccessManager::NetworkAccessibility t_accessible);
    void watchNetworkState(bool isOnline);

private:

    QStack<TByteCount> m_lastRecordedDownloadCounts;

    void startDownload();
    QVector<QByteArray> processChunks();
    void restartDownload();

    void waitForDownload();

    bool validateReceivedData();
    bool shouldStop();
    bool isConnectionStalled();

    void writeDownloadedFile(const QString& t_filePath) const;

    // PARAMS

    int m_lastValidChunkIndex;

    // PARAMS

    const ContentSummary& m_contentSummary;

    const int& getChunkSize() const;

    int m_maxNumberOfAttepmts;
    int m_requestTimeoutMsec;

    QNetworkRequest* m_currentRequest;
    QNetworkAccessManager* m_networkManager;
    QNetworkReply* m_currentReply;

    long long m_bytesDownloadedSoFar;

    void waitForReply() const;

    CancellationToken m_cancellationToken;

    QVector<QByteArray> m_chunks;
};

#endif // CHUNKEDDOWNLOADER_H
