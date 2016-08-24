/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "downloader.h"

#include "logger.h"
#include "timeoutexception.h"

void Downloader::downloadFile(const QString& t_urlPath, const QString& t_filePath, int t_requestTimeoutMsec, CancellationToken t_cancellationToken) const
{
    std::shared_ptr<QNetworkAccessManager> accessManager;
    std::shared_ptr<QNetworkReply> reply;

    fetchReply(t_urlPath, accessManager, reply);

    connect(reply.get(), &QNetworkReply::downloadProgress, this, &Downloader::downloadProgressChanged);

    waitForReply(reply, t_requestTimeoutMsec, t_cancellationToken);
    validateReply(reply);

    waitForFileDownload(reply, t_cancellationToken);
    writeDownloadedFile(reply, t_filePath, t_cancellationToken);

    disconnect(reply.get(), &QNetworkReply::downloadProgress, this, &Downloader::downloadProgressChanged);
}

QString Downloader::downloadString(const QString& t_urlPath, int t_requestTimeoutMsec, CancellationToken t_cancellationToken) const
{
    std::shared_ptr<QNetworkAccessManager> accessManager;
    std::shared_ptr<QNetworkReply> reply;

    fetchReply(t_urlPath, accessManager, reply);
    waitForReply(reply, t_requestTimeoutMsec, t_cancellationToken);
    validateReply(reply);

    return reply->readAll();
}

void Downloader::fetchReply(const QString& t_urlPath, std::shared_ptr<QNetworkAccessManager>& t_accessManager, std::shared_ptr<QNetworkReply>& t_reply) const
{
    logInfo("Fetching network reply.");

    QUrl url(t_urlPath);
    t_accessManager = std::make_shared<QNetworkAccessManager>();
    t_reply = std::shared_ptr<QNetworkReply>(t_accessManager.get()->get(QNetworkRequest(url)));
}

void Downloader::waitForReply(std::shared_ptr<QNetworkReply>& t_reply, int t_requestTimeoutMsec, CancellationToken t_cancellationToken) const
{
    logInfo("Waiting for network reply to be ready.");

    if (!t_reply->waitForReadyRead(10))
    {
        QEventLoop readyReadLoop;

        QTimer timeoutTimer;

        connect(t_reply.get(), &QNetworkReply::readyRead, &readyReadLoop, &QEventLoop::quit);
        connect(&t_cancellationToken, &CancellationToken::cancelled, &readyReadLoop, &QEventLoop::quit);
        connect(&timeoutTimer, &QTimer::timeout, &readyReadLoop, &QEventLoop::quit);

        timeoutTimer.setInterval(t_requestTimeoutMsec);
        timeoutTimer.setSingleShot(true);
        timeoutTimer.start();

        readyReadLoop.exec();

        t_cancellationToken.throwIfCancelled();

        if (!timeoutTimer.isActive())
        {
            throw TimeoutException();
        }
    }
}

void Downloader::validateReply(std::shared_ptr<QNetworkReply>& t_reply) const
{
    logInfo("Validating network reply.");

    if (t_reply.get()->error() != QNetworkReply::NoError)
    {
        throw Exception(t_reply.get()->errorString());
    }

    QVariant statusCode = t_reply.get()->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if (!statusCode.isValid())
    {
        throw Exception("Couldn't read HTTP status code from reply.");
    }

    int statusCodeValue = statusCode.toInt();

    if (statusCodeValue != 200)
    {
        throw Exception(QString("Invaild reply HTTP status code - %1").arg(statusCodeValue));
    }

    logDebug("Reply status code - %1", .arg(statusCodeValue));
}

void Downloader::waitForFileDownload(std::shared_ptr<QNetworkReply>& t_reply, CancellationToken t_cancellationToken) const
{
    logInfo("Waiting for file download.");

    QEventLoop finishedLoop;

    connect(t_reply.get(), &QNetworkReply::finished, &finishedLoop, &QEventLoop::quit);
    connect(&t_cancellationToken, &CancellationToken::cancelled, &finishedLoop, &QEventLoop::quit);

    finishedLoop.exec();

    t_cancellationToken.throwIfCancelled();
}

void Downloader::writeDownloadedFile(std::shared_ptr<QNetworkReply>& t_reply, const QString& t_filePath, CancellationToken t_cancellationToken) const
{
    logInfo("Writing downloaded data to file - %1", .arg(t_filePath));

    QFile file(t_filePath);

    if (!file.open(QFile::WriteOnly))
    {
        throw Exception("Couldn't open file for download.");
    }

    qint64 bufferSize = 4096;
    std::unique_ptr<char> buffer(new char[bufferSize]);

    while (!t_reply->atEnd())
    {
        t_cancellationToken.throwIfCancelled();

        qint64 readSize = t_reply->read(buffer.get(), bufferSize);
        if (readSize > 0)
        {
            file.write(buffer.get(), readSize);
        }
    }

    file.close();
}
