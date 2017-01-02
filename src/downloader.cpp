/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "downloader.h"

#include "logger.h"
#include "timeoutexception.h"

void Downloader::downloadFile(const QString& t_urlPath, const QString& t_filePath, int t_requestTimeoutMsec, CancellationToken t_cancellationToken)
{
    TSharedNetworkAccessManager accessManager;
    TSharedNetworkReply reply;

    fetchReply(t_urlPath, accessManager, reply);

    connect(reply.data(), &QNetworkReply::downloadProgress, this, &Downloader::downloadProgressChanged);

    waitForReply(reply, t_requestTimeoutMsec, t_cancellationToken);
    validateReply(reply);

    waitForFileDownload(reply, t_cancellationToken);
    writeDownloadedFile(reply, t_filePath, t_cancellationToken);

    disconnect(reply.data(), &QNetworkReply::downloadProgress, this, &Downloader::downloadProgressChanged);
}

QString Downloader::downloadString(const QString& t_urlPath, int t_requestTimeoutMsec, int& t_replyStatusCode, CancellationToken t_cancellationToken) const
{
    TSharedNetworkAccessManager accessManager;
    TSharedNetworkReply reply;

    fetchReply(t_urlPath, accessManager, reply);
    waitForReply(reply, t_requestTimeoutMsec, t_cancellationToken);
    validateReply(reply);

    waitForFileDownload(reply, t_cancellationToken);

    t_replyStatusCode = getReplyStatusCode(reply);

    return reply->readAll();
}

void Downloader::fetchReply(const QString& t_urlPath, TSharedNetworkAccessManagerRef t_accessManager, TSharedNetworkReplyRef t_reply) const
{
    QUrl url(t_urlPath);

    fetchReply(QNetworkRequest(url), t_accessManager, t_reply);
}

void Downloader::fetchReply(const QNetworkRequest& t_urlRequest, TSharedNetworkAccessManagerRef t_accessManager, TSharedNetworkReplyRef t_reply) const
{
    logInfo("Fetching network reply.");

    if (!t_reply.isNull())
    {
        t_reply->deleteLater();
        t_reply.reset(nullptr);
    }

    if (t_accessManager.isNull())
    {
        t_accessManager = TSharedNetworkAccessManager(new QNetworkAccessManager());
    }

    t_reply = TSharedNetworkReply(t_accessManager->get(t_urlRequest));
}

void Downloader::waitForReply(TSharedNetworkReplyRef t_reply, int t_requestTimeoutMsec, CancellationToken t_cancellationToken) const
{
    logInfo("Waiting for network reply to be ready.");

    if (!t_reply->waitForReadyRead(10))
    {
        QEventLoop readyReadLoop;

        QTimer timeoutTimer;

        connect(t_reply.data(), &QNetworkReply::readyRead, &readyReadLoop, &QEventLoop::quit);
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

void Downloader::validateReply(TSharedNetworkReplyRef t_reply) const
{
    logInfo("Validating network reply.");

    if (t_reply->error() != QNetworkReply::NoError)
    {
        throw std::runtime_error(t_reply->errorString().toStdString());
    }
}

int Downloader::getReplyStatusCode(TSharedNetworkReplyRef t_reply) const
{
    QVariant statusCode = t_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if (!statusCode.isValid())
    {
        throw std::runtime_error("Couldn't read HTTP status code from reply.");
    }

    int statusCodeValue = statusCode.toInt();

    logDebug("Reply status code - %1", .arg(statusCodeValue));

    return statusCodeValue;
}

void Downloader::waitForFileDownload(TSharedNetworkReplyRef t_reply, CancellationToken t_cancellationToken) const
{
    logInfo("Waiting for file download.");

    if (t_reply->isFinished())
    {
        return;
    }

    QEventLoop finishedLoop;

    connect(t_reply.data(), &QNetworkReply::finished, &finishedLoop, &QEventLoop::quit);
    connect(&t_cancellationToken, &CancellationToken::cancelled, &finishedLoop, &QEventLoop::quit);

    finishedLoop.exec();

    t_cancellationToken.throwIfCancelled();
}

void Downloader::writeDownloadedFile(TSharedNetworkReplyRef t_reply, const QString& t_filePath, CancellationToken t_cancellationToken) const
{
    logInfo("Writing downloaded data to file - %1", .arg(t_filePath));

    QFile file(t_filePath);

    if (!file.open(QFile::WriteOnly))
    {
        throw std::runtime_error("Couldn't open file for download.");
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

void Downloader::writeDownloadedData(const QByteArray& t_data, const QString& t_filePath, CancellationToken t_cancellationToken) const
{
    logInfo("Writing downloaded data to file - %1", .arg(t_filePath));

    QFile file(t_filePath);

    if (!file.open(QFile::WriteOnly))
    {
        throw std::runtime_error("Couldn't open file for download.");
    }

    qint64 bufferSize = 4096;
    std::unique_ptr<char> buffer(new char[bufferSize]);

    file.write(t_data);

    file.close();
}

