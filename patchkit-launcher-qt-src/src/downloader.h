/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QtNetwork>
#include <memory>

#include "cancellationtoken.h"

typedef long long TByteCount;

enum class DownloadError
{
    ConnectionIssues,
    ContentUnavailable
};

class Downloader : public QObject
{
    Q_OBJECT

public:
    typedef QNetworkAccessManager* TDataSource;

    typedef QSharedPointer<QNetworkReply> TRemoteDataReply;

    Downloader(const QString& t_resourceUrl, TDataSource t_dataSource, CancellationToken& t_cancellationToken);
    ~Downloader();

    void start();

    int  getStatusCode();

    void setRange(int t_bytesStart, int t_bytesEnd = -1);

    void waitUntilReadyRead();
    void waitUntilFinished();

    bool wasStarted() const;
    bool isFinished() const;
    bool isRunning() const;

    QString debugInfo() const;
    QString debugName() const;

    QByteArray readData();

    static bool doesStatusCodeIndicateSuccess(int t_statusCode);
    static bool checkInternetConnection();

signals:
    void downloadProgressChanged(TByteCount t_bytesDownloaded, TByteCount t_totalBytes) const;
    void downloadStarted(int t_statusCode);
    void downloadFinished();
    void downloadError(QNetworkReply::NetworkError t_errorCode);

    void timeout();

public slots:
    void stop();

private slots:
    void readyReadRelay();
    void finishedRelay();
    void errorRelay(QNetworkReply::NetworkError t_errorCode);

private:
    void fetchReply(const QNetworkRequest& t_urlRequest, TRemoteDataReply& t_reply) const;

    void validateReply(TRemoteDataReply& t_reply) const;

    int  getReplyStatusCode(TRemoteDataReply& t_reply) const;

    void waitForDownloadToFinish(TRemoteDataReply& t_reply) const;
    void waitForReadyRead(TRemoteDataReply& t_reply) const;

    bool m_isActive;

    QNetworkRequest     m_resourceRequest;
    TDataSource         m_remoteDataSource;
    TRemoteDataReply    m_remoteDataReply;
    CancellationToken   m_cancellationToken;
};
