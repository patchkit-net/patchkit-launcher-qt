/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "downloader.h"

#include "logger.h"
#include "timeoutexception.h"
#include "config.h"

Q_DECLARE_METATYPE(DownloadError)
int downloadErrorMetaTypeId = qRegisterMetaType<DownloadError>();

Downloader::Downloader
    (const QString& t_resourceUrl,
     TDataSource t_dataSource,
     CancellationToken& t_cancellationToken)
    : m_remoteDataSource(t_dataSource)
    , m_cancellationToken(t_cancellationToken)
    , m_resourceRequest(QUrl(t_resourceUrl))
    , m_isActive(false)
{
}

Downloader::~Downloader()
{
    if (!m_remoteDataReply.isNull())
    {
        if (m_remoteDataReply->isRunning())
        {
            m_remoteDataReply->abort();
        }
    }
}

void Downloader::start()
{
    logInfo("Downloader %1 - Start.", .arg(debugName()));

    fetchReply(m_resourceRequest, m_remoteDataReply);

    connect(m_remoteDataReply.data(), &QNetworkReply::readyRead, this, &Downloader::readyReadRelay);
    connect(m_remoteDataReply.data(), &QNetworkReply::finished, this, &Downloader::finishedRelay);

    connect(m_remoteDataReply.data(),
            static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &Downloader::errorRelay);

    connect(&m_cancellationToken, &CancellationToken::cancelled, m_remoteDataReply.data(), &QNetworkReply::abort);

    connect(m_remoteDataReply.data(), &QNetworkReply::downloadProgress, this, &Downloader::downloadProgressChanged);
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

bool Downloader::wasStarted() const
{
    return !m_remoteDataReply.isNull();
}

bool Downloader::isFinished() const
{
    return !m_remoteDataReply.isNull() && m_remoteDataReply->isFinished();
}

bool Downloader::isRunning() const
{
    return !m_remoteDataReply.isNull() && m_remoteDataReply->isRunning() && m_isActive;
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
    sb.append(QString("Reply status: %1 \n").arg(m_remoteDataReply.isNull() ? "Null" : "Exists"));

    return sb;
}

void Downloader::readyReadRelay()
{
    m_isActive = true;

    emit downloadStarted(getStatusCode());

    disconnect(m_remoteDataReply.data(), &QNetworkReply::readyRead, this, &Downloader::readyReadRelay);
}

void Downloader::errorRelay(QNetworkReply::NetworkError t_errorCode)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<QNetworkReply::NetworkError>();
    logWarning("Downloader %1 - Network reply encountered an error: %2",
               .arg(debugName(), metaEnum.valueToKey(t_errorCode)));

    emit downloadError(t_errorCode);
}

void Downloader::finishedRelay()
{
    logInfo("Downloader %1 - Downloader is finishing...", .arg(debugName()));

    if (m_cancellationToken.isCancelled())
    {
        logInfo("Finished by cancellatoin, will not emit finished signal.");
        return;
    }

    if (m_remoteDataReply.isNull())
    {
        logWarning("Finished but network reply is null, will not emit finished signal.");
        return;
    }

    if (!m_isActive)
    {
        logInfo("Finished but is not active, will not emit finished signal.");
        return;
    }

    logInfo("Emitting finished signal.");
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
    logDebug("Downloader %1 - Stopping.", .arg(debugName()));

    m_isActive = false;
    if (m_remoteDataReply.isNull())
        return;

    m_remoteDataReply->abort();

    m_remoteDataReply->deleteLater();

    m_remoteDataReply.reset(nullptr);
}

void Downloader::fetchReply(const QNetworkRequest& t_urlRequest, TRemoteDataReply& t_reply) const
{
    logInfo("Downloader %1 - Fetching network reply - URL: %2.",
            .arg(debugName(), t_urlRequest.url().toString()));

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
        logCritical("No remote data source provided.");
        return;
    }

    QNetworkReply* reply = m_remoteDataSource->get(t_urlRequest);

    if (!reply)
    {
        logCritical("Reply was null");
        return;
    }

    t_reply = TRemoteDataReply(reply);
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
    if (t_reply.isNull())
    {
        //throw std::runtime_error("Tried reading status code from a null reply.");
        return -1;
    }

    QVariant statusCode = t_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if (!statusCode.isValid())
    {
        //throw std::runtime_error("Couldn't read HTTP status code from reply.");
        return -1;
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
