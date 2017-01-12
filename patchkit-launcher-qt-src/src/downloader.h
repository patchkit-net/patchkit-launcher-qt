/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QtNetwork>
#include <memory>

#include "cancellationtoken.h"

/**
 * @brief The Downloader class
 *
 * Downloader shouldn't deal with QNetwork classes directly.
 * It should only deal with RemoteDataSource and RemoteDataReply
 */
class Downloader : public QObject
{
    Q_OBJECT

public:
    Downloader(QNetworkAccessManager* t_dataSource);

    typedef QSharedPointer<QNetworkReply>          TRemoteDataReply;
    typedef QSharedPointer<QNetworkAccessManager>  TRemoteDataSource;

    typedef long long TByteCount;

    virtual QByteArray downloadFile(const QString& t_urlPath, int t_requestTimeoutMsec, CancellationToken t_cancellationToken);
    QByteArray downloadFile(const QNetworkRequest& t_request, int t_requestTimeoutMsec, CancellationToken t_cancellationToken);

    QString downloadString(const QString& t_urlPath, int t_requestTimeoutMsec, int& t_replyStatusCode, CancellationToken t_cancellationToken) const;

signals:
    void downloadProgressChanged(const TByteCount& t_bytesDownloaded, const TByteCount& t_totalBytes);
    void terminate();

protected:
    void fetchReply(const QString& t_urlPath, TRemoteDataReply& t_reply) const;
    void fetchReply(const QNetworkRequest& t_urlRequest, TRemoteDataReply& t_reply) const;

    void waitForReply(TRemoteDataReply& t_reply, int t_requestTimeoutMsec, CancellationToken t_cancellationToken) const;

    void validateReply(TRemoteDataReply& t_reply) const;

    int getReplyStatusCode(TRemoteDataReply& t_reply) const;

    void waitForFileDownload(TRemoteDataReply& t_reply, CancellationToken t_cancellationToken) const;

    void restartDownload(TRemoteDataReply& t_reply, const QUrl& t_url) const;
    void restartDownload(TRemoteDataReply& t_reply, const QNetworkRequest& t_request) const;

private:
    QNetworkAccessManager* m_remoteDataSource;
};
