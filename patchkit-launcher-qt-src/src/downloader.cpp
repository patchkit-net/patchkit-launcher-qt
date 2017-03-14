/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "downloader.h"

#include "logger.h"
#include "timeoutexception.h"
#include "config.h"

Downloader::Downloader
    (const QString& t_resourceUrl,
     TDataSource t_dataSource,
     CancellationToken& t_cancellationToken)
    : m_remoteDataSource(t_dataSource)
    , m_cancellationToken(t_cancellationToken)
    , m_resourceRequest(QUrl(t_resourceUrl))
{
}

void Downloader::start()
{
    fetchReply(m_resourceRequest, m_remoteDataReply);

    connect(m_remoteDataReply.data(), &QNetworkReply::readyRead, this, &Downloader::readyReadRelay);
    connect(m_remoteDataReply.data(), &QNetworkReply::finished, this, &Downloader::finishedRelay);

    connect(m_remoteDataReply.data(),
            static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &Downloader::errorRelay);

    connect(&m_cancellationToken, &CancellationToken::cancelled, m_remoteDataReply.data(), &QNetworkReply::abort);
}

int Downloader::getStatusCode()
{
    return getReplyStatusCode(m_remoteDataReply);
}

void Downloader::setRange(int t_bytesStart, int t_bytesEnd)
{
    QByteArray header = "bytes=" + QByteArray::number(t_bytesStart) + "-";

    if (t_bytesEnd != -1 && t_bytesEnd > t_bytesStart)
    {
        header += QByteArray::number(t_bytesEnd);
    }

    m_resourceRequest.setRawHeader("Range", header);
}

QByteArray Downloader::readData()
{
    return m_remoteDataReply->readAll();
}

void Downloader::waitUntilFinished()
{
    waitForDownloadToFinish(m_remoteDataReply);
}

void Downloader::readyReadRelay()
{
    emit downloadStarted();
    disconnect(m_remoteDataReply.data(), &QNetworkReply::readyRead, this, &Downloader::readyReadRelay);

    connect(m_remoteDataReply.data(), &QNetworkReply::downloadProgress, this, &Downloader::downloadProgressChanged);
}

void Downloader::errorRelay(QNetworkReply::NetworkError t_errorCode)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<QNetworkReply::NetworkError>();
    logWarning("Network reply encountered an error: %1", .arg(metaEnum.valueToKey(t_errorCode)));

    emit downloadError(t_errorCode);
}

void Downloader::finishedRelay()
{
    emit downloadFinished();
}

bool Downloader::doesStatusCodeIndicateSuccess(int t_statusCode)
{
    return t_statusCode >= 200 && t_statusCode < 300;
}

bool Downloader::checkInternetConnection()
{
    QProcess proc;
    QString exec = "ping";
    QStringList params = {Config::pingTarget, Config::pingCountArg, "1"};

    proc.start(exec, params);

    if (!proc.waitForFinished())
    {
        return false;
    }

    if (proc.exitCode() == 0)
    {
        return true;
    }
    else
    {
        logWarning("No internet connection.");
        QString p_stdout = proc.readAllStandardOutput();
        logInfo("Ping output was: %1", .arg(p_stdout));

        return false;
    }
}

void Downloader::stop()
{
    m_remoteDataReply->abort();
}

void Downloader::fetchReply(const QNetworkRequest& t_urlRequest, TRemoteDataReply& t_reply) const
{
    logInfo("Fetching network reply - URL: %1.", .arg(t_urlRequest.url().toString()));

    if (!t_reply.isNull())
    {
        if (!t_reply->isFinished())
        {
            t_reply->abort();
        }

        t_reply->deleteLater();
        t_reply.reset(nullptr);
    }

    if (!m_remoteDataSource)
    {
        throw std::runtime_error("No remote data source provided.");
    }

    QNetworkReply* reply = m_remoteDataSource->get(t_urlRequest);

    if (!reply)
    {
        throw std::runtime_error("Reply was null.");
    }

    t_reply = TRemoteDataReply(reply);
}

void Downloader::waitForReply(TRemoteDataReply& t_reply, int t_requestTimeoutMsec) const
{
    logInfo("Waiting for network reply to be ready.");

    if (t_reply->isFinished())
    {
        return;
    }

    if (!t_reply->waitForReadyRead(10))
    {
        QEventLoop readyReadLoop;

        QTimer timeoutTimer;

        connect(t_reply.data(), &QNetworkReply::readyRead, &readyReadLoop, &QEventLoop::quit);
        connect(&m_cancellationToken, &CancellationToken::cancelled, &readyReadLoop, &QEventLoop::quit);
        connect(&timeoutTimer, &QTimer::timeout, &readyReadLoop, &QEventLoop::quit);

        timeoutTimer.setInterval(t_requestTimeoutMsec);
        timeoutTimer.setSingleShot(true);
        timeoutTimer.start();

        readyReadLoop.exec();

        m_cancellationToken.throwIfCancelled();

        if (!timeoutTimer.isActive())
        {
            throw TimeoutException();
        }
    }
}

void Downloader::validateReply(TRemoteDataReply& t_reply) const
{
    logInfo("Validating network reply.");

    if (t_reply->error() != QNetworkReply::NoError)
    {
        throw std::runtime_error(t_reply->errorString().toStdString());
    }
}

int Downloader::getReplyStatusCode(TRemoteDataReply& t_reply) const
{
    QVariant statusCode = t_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if (!statusCode.isValid())
    {
        throw std::runtime_error("Couldn't read HTTP status code from reply.");
    }

    int statusCodeValue = statusCode.toInt();

    return statusCodeValue;
}

void Downloader::waitForDownloadToFinish(TRemoteDataReply& t_reply) const
{
    logInfo("Waiting for download to finish.");

    if (t_reply->isFinished())
    {
        return;
    }

    QEventLoop finishedLoop;

    connect(t_reply.data(), &QNetworkReply::finished, &finishedLoop, &QEventLoop::quit);
    connect(&m_cancellationToken, &CancellationToken::cancelled, &finishedLoop, &QEventLoop::quit);

    finishedLoop.exec();

    m_cancellationToken.throwIfCancelled();
}
