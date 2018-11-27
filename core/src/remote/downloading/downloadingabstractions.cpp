#include "downloadingabstractions.h"
#include <QTimer>
#include <QNetworkReply>
#include <QEventLoop>

int downloading::abstractions::getReplyStatusCode(const QNetworkReply* reply)
{
    if (!reply)
    {
        return -1;
    }

    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if (!statusCode.isValid())
    {
        return -1;
    }

    int statusCodeValue = statusCode.toInt();

    return statusCodeValue;
}

bool downloading::abstractions::tryRangedDownload(QNetworkAccessManager &nam, QUrl url, data::DownloadRange range, QIODevice &target, int timeout, CancellationToken cancellationToken)
{
    QNetworkRequest request(url);
    request.setRawHeader("Range", range.toString().toUtf8());
    QNetworkReply* reply = nam.get(request);

    QObject::connect(&cancellationToken, &CancellationToken::cancelled, reply, &QNetworkReply::abort);

    reply->waitForReadyRead(timeout);

    int statusCode = getReplyStatusCode(reply);

    if (!doesStatusCodeIndicateSuccess(statusCode))
    {
        reply->abort();
        return false;
    }

    while (reply->waitForReadyRead(timeout))
    {
        cancellationToken.throwIfCancelled();
        target.write(reply->readAll());
    }

    cancellationToken.throwIfCancelled();

    return true;
}

bool downloading::abstractions::tryDownload(QNetworkAccessManager &nam, QString stringUrl, QIODevice &target, int timeout, CancellationToken cancellationToken)
{
    QUrl url(stringUrl);

    return tryDownload(nam, url, target, timeout, cancellationToken);
}

bool downloading::abstractions::tryDownload(
        QNetworkAccessManager& nam, QUrl url, QIODevice& target, int timeout, CancellationToken cancellationToken)
{
    qInfo() << "Downloading from " << url.toString() << " with timeout " << timeout;

    QNetworkRequest request(url);

    QEventLoop loop;

    QNetworkReply* reply = nam.get(request);

    QObject::connect(&cancellationToken, &CancellationToken::cancelled, reply, &QNetworkReply::abort);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    loop.exec();
    cancellationToken.throwIfCancelled();

    int statusCode = getReplyStatusCode(reply);

    if (!doesStatusCodeIndicateSuccess(statusCode))
    {
        qWarning() << "Status code was " << statusCode;
        return false;
    }

    auto data = reply->readAll();
    target.write(data);
    target.close();

    return true;
}

bool downloading::abstractions::doesStatusCodeIndicateSuccess(int statusCode)
{
    return statusCode >= 200 && statusCode < 300;
}
