#include "patchkitremotepatcher.h"
#include "launcherexception.h"
#include <QtNetwork/QtNetwork>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

int PatchKitRemotePatcher::getVersion(const QString &patcherSecret)
{
    QString jsonData = downloadString(QString("http://api.patchkit.net/1/apps/%1/versions/latest/id").arg(patcherSecret));

    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData.toUtf8());

    if(!jsonDocument.isObject())
    {
        throw LauncherException("Couldn't read version id from JSON data.");
    }

    QJsonObject jsonObject = jsonDocument.object();

    if(!jsonObject.contains("id"))
    {
        throw LauncherException("Couldn't read version id from JSON data.");
    }

    int idValue = jsonObject.value("id").toInt(-1);

    if(idValue == -1)
    {
        throw LauncherException(QString("Couldn't read version id from JSON data."));
    }

    return idValue;
}

QString PatchKitRemotePatcher::download(const QString &patcherSecret, const int &version)
{
    QString* contentUrls = getContentUrls(patcherSecret, version);

    QString filePath("file.zip");

    downloadFile(filePath, contentUrls[0]);

    delete[] contentUrls;

    return filePath;
}

void PatchKitRemotePatcher::downloadProgress(const long long &bytesReceived, const long long &bytesTotal)
{
    emit bytesDownloadedChanged(bytesReceived);
    emit totalBytesChanged(bytesTotal);
}

QString *PatchKitRemotePatcher::getContentUrls(const QString &patcherSecret, const int &version)
{
    QString jsonData = downloadString(QString("http://api.patchkit.net/1/apps/%1/versions/%2/content_urls").arg(patcherSecret, QString::number(version)));

    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData.toUtf8());

    if(!jsonDocument.isArray())
    {
        throw LauncherException("Couldn't read content urls from JSON data.");
    }

    QJsonArray jsonArray = jsonDocument.array();

    if(jsonArray.size() == 0)
    {
        throw LauncherException("Empty content urls.");
    }

    QString *result = new QString[jsonArray.size()];

    for(int i = 0; i < jsonArray.size(); i++)
    {
        if(!jsonArray[i].isObject())
        {
            throw LauncherException("Couldn't read content urls from JSON data.");
        }

        QJsonObject jsonObject = jsonArray[i].toObject();

        if(!jsonObject.contains("url"))
        {
            throw LauncherException("Couldn't read content urls from JSON data.");
        }

        QJsonValue jsonValue = jsonObject.value("url");

        if(!jsonValue.isString())
        {
            throw LauncherException("Couldn't read content urls from JSON data.");
        }

        result[i] = jsonValue.toString();
    }

    return result;
}

void PatchKitRemotePatcher::getNetworkReply(const QString &urlPath, QNetworkAccessManager*& accessManager, QNetworkReply*& reply)
{
    qDebug() << QString("Getting network reply from %1").arg(urlPath).toStdString().c_str();
    QUrl url(urlPath);
    accessManager = new QNetworkAccessManager();
    reply = accessManager->get( QNetworkRequest(url));

    QEventLoop* readyReadLoop = new QEventLoop();
    QObject::connect(reply, &QNetworkReply::readyRead, readyReadLoop, &QEventLoop::quit);

    QTimer timeoutTimer;
    connect(&timeoutTimer, &QTimer::timeout, readyReadLoop, &QEventLoop::quit);
    timeoutTimer.setInterval(downloadTimeoutInSeconds *1000);

    timeoutTimer.start();
    readyReadLoop->exec();

    delete readyReadLoop;

    if(timeoutTimer.remainingTime() == 0)
    {
        throw LauncherException("Request timeout.");
    }

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

    qDebug() << QString("Reply status code - %1").arg(statusCodeValue).toStdString().c_str();
}

QString PatchKitRemotePatcher::downloadString(const QString &urlPath)
{
    QNetworkAccessManager *accessManager;
    QNetworkReply *reply;
    getNetworkReply(urlPath, accessManager, reply);

    QString result = QString(reply->readAll());

    qDebug() << result;

    delete reply;
    delete accessManager;

    return result;
}

void PatchKitRemotePatcher::downloadFile(const QString &filePath, const QString &urlPath)
{
    QNetworkAccessManager *accessManager;
    QNetworkReply *reply;
    getNetworkReply(urlPath, accessManager, reply);

    QFile file(filePath);

    if(!file.open(QFile::WriteOnly))
    {
        throw LauncherException("Couldn't open file for download.");
    }

    file.write(reply->readAll());

    file.flush();
    file.close();

    delete reply;
    delete accessManager;
}
