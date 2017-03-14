/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QtNetwork>
#include <memory>

#include "cancellationtoken.h"

class Downloader : public QObject
{
    Q_OBJECT

public:
    typedef QNetworkAccessManager* TDataSource;

    typedef long long TByteCount;

    typedef QSharedPointer<QNetworkReply> TRemoteDataReply;

    Downloader(const QString& t_resourceUrl, TDataSource t_dataSource, CancellationToken& t_cancellationToken);

    void start();
    int getStatusCode();
    void setRange(int t_bytesStart, int t_bytesEnd = -1);

    QByteArray readData();

    void waitUntilFinished();

    static bool doesStatusCodeIndicateSuccess(int t_statusCode);
    static bool checkInternetConnection();

signals:
    void downloadProgressChanged(const TByteCount& t_bytesDownloaded, const TByteCount& t_totalBytes) const;
    void downloadStarted(); // This method signals when the first portion of bytes was received.
    void downloadFinished(); // Signals that the download has finished without errors
    void downloadError(QNetworkReply::NetworkError t_errorCode); // Signals that the download was stopped for some reason

public slots:
    void stop();

private slots:
    void readyReadRelay();
    void finishedRelay();
    void errorRelay(QNetworkReply::NetworkError t_errorCode);

private:
    void fetchReply(const QNetworkRequest& t_urlRequest, TRemoteDataReply& t_reply) const;

    void waitForReply(TRemoteDataReply& t_reply, int t_requestTimeoutMsec) const;

    void validateReply(TRemoteDataReply& t_reply) const;

    int  getReplyStatusCode(TRemoteDataReply& t_reply) const;

    void waitForDownloadToFinish(TRemoteDataReply& t_reply) const;

    QNetworkRequest     m_resourceRequest;
    TDataSource         m_remoteDataSource;
    TRemoteDataReply    m_remoteDataReply;
    CancellationToken   m_cancellationToken;
};
