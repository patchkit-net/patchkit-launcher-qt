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
    void downloadFile(const QString& t_urlPath, const QString& t_filePath, int t_requestTimeoutMsec, CancellationToken t_cancellationToken) const;

    QString downloadString(const QString& t_urlPath, int t_requestTimeoutMsec, CancellationToken t_cancellationToken) const;

signals:
    void downloadProgressChanged(const long long& t_bytesDownloaded, const long long& t_totalBytes);

private:
    void fetchReply(const QString& t_urlPath, std::shared_ptr<QNetworkAccessManager>& t_accessManager, std::shared_ptr<QNetworkReply>& t_reply) const;

    void waitForReply(std::shared_ptr<QNetworkReply>& t_reply, int t_requestTimeoutMsec, CancellationToken t_cancellationToken) const;

    void validateReply(std::shared_ptr<QNetworkReply>& t_reply) const;

    void waitForFileDownload(std::shared_ptr<QNetworkReply>& t_reply, CancellationToken t_cancellationToken) const;

    void writeDownloadedFile(std::shared_ptr<QNetworkReply>& t_reply, const QString& t_filePath, CancellationToken t_cancellationToken) const;
};
