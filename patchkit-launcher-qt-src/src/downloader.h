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
    typedef QSharedPointer<QNetworkReply>           TSharedNetworkReply;
    typedef QSharedPointer<QNetworkAccessManager>   TSharedNetworkAccessManager;

    typedef long long TByteCount;

    virtual void downloadFile(const QString& t_urlPath, const QString& t_filePath, int t_requestTimeoutMsec, CancellationToken t_cancellationToken);

    QString downloadString(const QString& t_urlPath, int t_requestTimeoutMsec, int& t_replyStatusCode, CancellationToken t_cancellationToken) const;

signals:
    void downloadProgressChanged(const TByteCount& t_bytesDownloaded, const TByteCount& t_totalBytes);

protected:
    void fetchReply(const QString& t_urlPath, TSharedNetworkAccessManager& t_accessManager, TSharedNetworkReply& t_reply) const;
    void fetchReply(const QNetworkRequest& t_urlRequest, TSharedNetworkAccessManager& t_accessManager, TSharedNetworkReply& t_reply) const;

    void waitForReply(TSharedNetworkReply& t_reply, int t_requestTimeoutMsec, CancellationToken t_cancellationToken) const;

    void validateReply(TSharedNetworkReply& t_reply) const;

    int getReplyStatusCode(TSharedNetworkReply& t_reply) const;

    void waitForFileDownload(TSharedNetworkReply& t_reply, CancellationToken t_cancellationToken) const;

    void writeDownloadedData(const QByteArray& t_data, const QString& t_filePath, CancellationToken t_cancellationToken) const;
    void writeDownloadedFile(TSharedNetworkReply& t_reply, const QString& t_filePath, CancellationToken t_cancellationToken) const;
};
