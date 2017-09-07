/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "downloader.h"

#include "logger.h"
#include "customexceptions.h"
#include "config.h"

Q_DECLARE_METATYPE(DownloadError)
int downloadErrorMetaTypeId = qRegisterMetaType<DownloadError>();
int tByteCountMetaTypeId = qRegisterMetaType<TByteCount>();

Downloader::Downloader
    (const QString& t_resourceUrl,
     TDataSource t_dataSource,
     CancellationToken& t_cancellationToken)
    : m_isActive(false)
    , m_resourceRequest(QUrl(t_resourceUrl))
    , m_remoteDataSource(t_dataSource)
    , m_cancellationToken(t_cancellationToken)
    , m_remoteDataReply(nullptr)
    , m_lastError(QNetworkReply::NoError)
{
}

void Downloader::start()
{
    if (m_remoteDataReply)
    {
        qInfo() << "Downloader " << debugName() << " has already been started.";
        return;
    }

    qInfo() << "Downloader " << debugName() << " - Start.";

    fetchReply(m_resourceRequest, m_remoteDataReply);

    connect(m_remoteDataReply, &QNetworkReply::readyRead, this, &Downloader::readyReadRelay);
    connect(m_remoteDataReply, &QNetworkReply::finished, this, &Downloader::finishedRelay);

    connect(m_remoteDataReply,
            static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &Downloader::errorRelay);

    connect(m_remoteDataReply, &QNetworkReply::downloadProgress, this, &Downloader::downloadProgressChanged);
    connect(&m_cancellationToken, &CancellationToken::cancelled, m_remoteDataReply, &QNetworkReply::abort);
}

void Downloader::restart()
{
    stop();
    start();
}

int Downloader::getStatusCode() const
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

void Downloader::waitUntilReadyRead()
{
    waitForReadyRead(m_remoteDataReply);
}

QByteArray Downloader::readData()
{
    if (!m_remoteDataReply)
    {
        return QByteArray();
    }

    return m_remoteDataReply->readAll();
}

void Downloader::waitUntilFinished()
{
    if (!wasStarted() || isFinished())
    {
        return;
    }

    QEventLoop waitUntilFinishedLoop;

    connect(this, &Downloader::downloadFinished, &waitUntilFinishedLoop, &QEventLoop::quit);
    connect(this, &Downloader::downloadError, &waitUntilFinishedLoop, &QEventLoop::quit);

    waitUntilFinishedLoop.exec();
}

bool Downloader::wasStarted() const
{
    return m_remoteDataReply != nullptr && !encounteredAnError();
}

bool Downloader::isFinished() const
{
    return wasStarted() && m_remoteDataReply->isFinished() && !encounteredAnError();
}

bool Downloader::isRunning() const
{
    return wasStarted() && m_remoteDataReply->isRunning() && m_isActive && !encounteredAnError();
}

bool Downloader::encounteredAnError() const
{
    auto statusCode = getStatusCode();

    if (doesStatusCodeIndicateSuccess(statusCode))
    {
        return false;
    }

    return m_lastError != QNetworkReply::NoError;
}

QString Downloader::debugName() const
{
    return QString::number((size_t) this, 16);
}

QString Downloader::debugInfo() const
{
    QString sb;
    sb.append(QString("Memory address: %1 \n").arg((size_t) this));
    sb.append(QString("Url: %1 \n").arg(m_resourceRequest.url().toString()));
    sb.append(QString("Reply status: %1 \n").arg(m_remoteDataReply ? "Null" : "Exists"));
    sb.append(QString("Active: %1 \n").arg(m_isActive ? "Yes" : "No"));

    return sb;
}

void Downloader::readyReadRelay()
{
    m_isActive = true;

    emit downloadStarted(getStatusCode());

    disconnect(m_remoteDataReply, &QNetworkReply::readyRead, this, &Downloader::readyReadRelay);
}

void Downloader::errorRelay(QNetworkReply::NetworkError t_errorCode)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<QNetworkReply::NetworkError>();

    qWarning()  << "Downloader "
                << debugName()
                << " - Network reply encountered an error: "
                << metaEnum.valueToKey(t_errorCode);

    m_lastError = t_errorCode;

    emit downloadError(t_errorCode);
}

void Downloader::finishedRelay()
{
    qInfo()     << "Downloader "
                << debugName()
                << " - Downloader is finishing...";

    if (m_cancellationToken.isCancelled())
    {
        qInfo("Finished by cancellation, will not emit finished signal.");
        return;
    }

    if (!m_remoteDataReply)
    {
        qWarning("Finished but network reply is null, will not emit finished signal.");
        return;
    }

    if (!m_remoteDataReply->isFinished())
    {
        qWarning("Finished but network reply is not finished, will not emit finished signal.");
        return;
    }

    if (!m_isActive)
    {
        qInfo("Finished but is not active, will not emit finished signal.");
        return;
    }

    qInfo("Emitting finished signal.");
    emit downloadFinished();
}

bool Downloader::doesStatusCodeIndicateSuccess(int t_statusCode)
{
    return t_statusCode >= 200 && t_statusCode < 300;
}

void Downloader::stop()
{
    qDebug() << "Downloader " << debugName() << " - Stopping.";

    m_isActive = false;
    if (!m_remoteDataReply)
    {
        return;
    }

    m_remoteDataReply->abort();
    m_remoteDataReply->deleteLater();
    m_remoteDataReply = nullptr;

    m_lastError = QNetworkReply::NoError;
}

void Downloader::fetchReply(const QNetworkRequest& t_urlRequest, TRemoteDataReply& t_reply) const
{
    qInfo() << "Downloader "
            << debugName()
            << " - Fetching network reply - URL: "
            << t_urlRequest.url().toString();

    if (!m_remoteDataSource)
    {
        qCritical("No remote data source provided.");
        return;
    }

    QNetworkReply* reply = m_remoteDataSource->get(t_urlRequest);

    if (!reply)
    {
        qCritical("Reply was null");
        return;
    }

    t_reply = reply;
}

void Downloader::validateReply(TRemoteDataReply& t_reply) const
{
    qInfo("Validating network reply.");

    if (t_reply->error() != QNetworkReply::NoError)
    {
        throw std::runtime_error(t_reply->errorString().toStdString());
    }
}

int Downloader::getReplyStatusCode(TRemoteDataReply t_reply)
{
    if (!t_reply)
    {
        return -1;
    }

    QVariant statusCode = t_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if (!statusCode.isValid())
    {
        return -1;
    }

    int statusCodeValue = statusCode.toInt();

    return statusCodeValue;
}

void Downloader::waitForDownloadToFinish(TRemoteDataReply& t_reply) const
{
    qInfo("Waiting for download to finish.");

    if (t_reply->isFinished())
    {
        return;
    }

    QEventLoop finishedLoop;

    connect(t_reply, &QNetworkReply::finished, &finishedLoop, &QEventLoop::quit);
    connect(&m_cancellationToken, &CancellationToken::cancelled, &finishedLoop, &QEventLoop::quit);

    finishedLoop.exec();

    m_cancellationToken.throwIfCancelled();
}

void Downloader::waitForReadyRead(Downloader::TRemoteDataReply& t_reply) const
{
    qInfo("Waiting for download to be ready.");

    if (t_reply->isFinished())
    {
        return;
    }

    QEventLoop readyReadLoop;

    connect(t_reply, &QNetworkReply::readyRead, &readyReadLoop, &QEventLoop::quit);
    connect(&m_cancellationToken, &CancellationToken::cancelled, &readyReadLoop, &QEventLoop::quit);

    readyReadLoop.exec();

    m_cancellationToken.throwIfCancelled();
}
