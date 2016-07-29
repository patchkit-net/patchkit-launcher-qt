#include "patchkitremotepatcher.h"
#include "launcherexception.h"
#include "QtNetwork/QtNetwork"

PatchKitRemotePatcher::PatchKitRemotePatcher()
{

}

int PatchKitRemotePatcher::getVersion(const QString &patcherSecret)
{
    QString test = downloadString(QString("http://api.patchkit.net/1/apps/%1/versions/latest/id").arg(patcherSecret));

    qDebug() << test;

    return -1;
}

QString PatchKitRemotePatcher::download(const QString &patcherSecret, const int &version)
{
    return QString("test");
}

QString PatchKitRemotePatcher::downloadString(const QString &urlPath)
{
    qDebug() << QString("Downloading string from %1").arg(urlPath).toStdString().c_str();
    QUrl url(urlPath);
    QNetworkAccessManager accessManager(this);
    QNetworkReply* reply = accessManager.get( QNetworkRequest(url));

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::readyRead, &loop, &QEventLoop::quit);

    QTimer idleTimer(this);
    connect(&idleTimer,&QTimer::timeout,&loop,&QEventLoop::quit);
    idleTimer.setInterval(downloadTimeoutInSeconds *1000);

    idleTimer.start();
    loop.exec();

    if(reply->error() != QNetworkReply::NoError)
    {
        throw LauncherException(reply->errorString());
    }

    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if(!statusCode.isValid())
    {
        throw LauncherException("Couldn't read HTTP status code from reply.");
    }

    int statusCodeValue = statusCode.toInt();

    if(statusCodeValue != 200)
    {
        throw LauncherException(QString("Invaild reply HTTP status code - %1").arg(statusCodeValue));
    }

    QString result = QString(reply->readAll());

    qDebug() << result;

    delete reply;

    return result;
}
