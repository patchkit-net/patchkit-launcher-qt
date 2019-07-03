#include "downloadingabstractions.h"

#include "cancellation/cancellationtoken.h"

#include <QTimer>
#include <QNetworkReply>
#include <QEventLoop>

namespace downloading
{
namespace abstractions
{

int getReplyStatusCode(const QNetworkReply* reply)
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

bool tryRangedDownload(
        QNetworkAccessManager& nam, QUrl url,
        data::DownloadRange range, QIODevice& target,
        int timeout, CancellationToken cancellationToken)
{
    url.setScheme("https");
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

bool tryRangedDownload(
        QNetworkAccessManager& nam, const QString& stringUrl,
        data::DownloadRange range, QIODevice& target,
        int timeout, CancellationToken cancellationToken)
{
    QUrl url(stringUrl);

    return tryRangedDownload(nam, url, range, target, timeout, cancellationToken);
}

int waitUntilReplyIsReady(
        QNetworkReply* reply,
        int timeout,
        CancellationToken cancellationToken)
{
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    QObject::connect(&cancellationToken, &CancellationToken::cancelled,
                     &loop, &QEventLoop::quit);

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

void waitUntilReplyIsFinished(
        QNetworkReply* reply, int timeout, CancellationToken cancellationToken)
{
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    QObject::connect(&cancellationToken, &CancellationToken::cancelled,
                     &loop, &QEventLoop::quit);

    QObject::connect(reply, &QNetworkReply::finished,
                     &loop, &QEventLoop::quit);

    QObject::connect(&timer, &QTimer::timeout,
                     &loop, &QEventLoop::quit);

    timer.start(timeout);
    loop.exec();

    cancellationToken.throwIfCancelled();

    if (timer.remainingTime() == 0)
    {
        throw Timeout("Waiting for reply to finish timed out");
    }
}

void bufferReply(
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

        if (!reply->isFinished())
        {
            QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
            QObject::connect(reply, &QNetworkReply::readyRead, &loop, &QEventLoop::quit);
            QObject::connect(&cancellationToken, &CancellationToken::cancelled,
                             &loop, &QEventLoop::quit);

            timer.start(timeout);
            loop.exec();
        }

        cancellationToken.throwIfCancelled();

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

bool tryDownload(
        QNetworkAccessManager &nam, const QString& stringUrl,
        QIODevice &target, int timeout, CancellationToken cancellationToken)
{
    QUrl url(stringUrl);

    return tryDownload(nam, url, target, timeout, cancellationToken);
}

bool tryDownload(
        QNetworkAccessManager& nam, QUrl url, QIODevice& target,
        int timeout, CancellationToken cancellationToken)
{
    url.setScheme("https");
    qInfo() << "Downloading from " << url.toString() << " with timeout " << timeout;

    QNetworkRequest request(url);
    QNetworkReply* reply = nam.get(request);

    int statusCode = waitUntilReplyIsReady(reply, timeout, cancellationToken);

    if (!doesStatusCodeIndicateSuccess(statusCode))
    {
        qWarning() << "Status code was " << statusCode;
        return false;
    }

    waitUntilReplyIsFinished(reply, timeout, cancellationToken);

    auto data = reply->readAll();
    target.write(data);

    return true;
}

bool doesStatusCodeIndicateSuccess(int statusCode)
{
    return statusCode >= 200 && statusCode < 300;
}

} // namespace abstractions
} // namespace downloading
