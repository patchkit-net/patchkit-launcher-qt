#include "downloadingabstractions.h"

#include "cancellation/cancellationtoken.h"

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

bool downloading::abstractions::tryRangedDownload(
        QNetworkAccessManager& nam, QUrl url,
        data::DownloadRange range, QIODevice& target,
        int timeout, CancellationToken cancellationToken)
{
    QNetworkRequest request(url);
    request.setRawHeader("Range", range.toString().toUtf8());
    QNetworkReply* reply = nam.get(request);

    int statusCode = waitUntilReplyIsReady(reply, timeout, cancellationToken);

    if (!doesStatusCodeIndicateSuccess(statusCode))
    {
        return false;
    }

    bufferReply(reply, target, timeout, cancellationToken);

    return true;
}

bool downloading::abstractions::tryRangedDownload(
        QNetworkAccessManager& nam, const QString& stringUrl,
        data::DownloadRange range, QIODevice& target,
        int timeout, CancellationToken cancellationToken)
{
    QUrl url(stringUrl);

    return tryRangedDownload(nam, url, range, target, timeout, cancellationToken);
}

int downloading::abstractions::waitUntilReplyIsReady(
        QNetworkReply* reply,
        int timeout,
        CancellationToken cancellationToken)
{
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    QObject::connect(&cancellationToken, &CancellationToken::cancelled,
                     reply, &QNetworkReply::abort);

    QObject::connect(reply, &QNetworkReply::readyRead, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer.start(timeout);
    loop.exec();

    cancellationToken.throwIfCancelled();

    if (timer.remainingTime() == 0)
    {
        throw Timeout("Waiting for reply timed out");
    }

    return getReplyStatusCode(reply);
}

void downloading::abstractions::bufferReply(
        QNetworkReply* reply, QIODevice& target,
        int timeout, CancellationToken cancellationToken)
{
    if (!target.isOpen() || !target.open(QIODevice::WriteOnly))
    {
        throw FatalException("Failed to open device for writing");
    }

    do
    {
        QEventLoop loop;
        QTimer timer;
        timer.setSingleShot(true);

        QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        QObject::connect(reply, &QNetworkReply::readyRead, &loop, &QEventLoop::quit);
        QObject::connect(&cancellationToken, &CancellationToken::cancelled,
                         &loop, &QEventLoop::quit);

        timer.start(timeout);
        loop.exec();

        QByteArray data = reply->readAll();

        if (timer.remainingTime() == 0)
        {
            throw Timeout("Buffering reply timed out");
        }

        target.write(data);
    }
    while (!reply->isFinished());

    target.close();
}

bool downloading::abstractions::tryDownload(
        QNetworkAccessManager &nam, const QString& stringUrl,
        QIODevice &target, int timeout, CancellationToken cancellationToken)
{
    QUrl url(stringUrl);

    return tryDownload(nam, url, target, timeout, cancellationToken);
}

bool downloading::abstractions::tryDownload(
        QNetworkAccessManager& nam, QUrl url, QIODevice& target,
        int timeout, CancellationToken cancellationToken)
{
    qInfo() << "Downloading from " << url.toString() << " with timeout " << timeout;

    QNetworkRequest request(url);
    QNetworkReply* reply = nam.get(request);

    int statusCode = waitUntilReplyIsReady(reply, timeout, cancellationToken);

    if (!doesStatusCodeIndicateSuccess(statusCode))
    {
        qWarning() << "Status code was " << statusCode;
        return false;
    }

    auto data = reply->readAll();
    target.write(data);

    return true;
}

bool downloading::abstractions::doesStatusCodeIndicateSuccess(int statusCode)
{
    return statusCode >= 200 && statusCode < 300;
}
