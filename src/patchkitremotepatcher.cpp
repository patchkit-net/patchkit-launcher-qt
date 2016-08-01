#include "patchkitremotepatcher.h"
#include "launcherexception.h"
#include "launchercancelledexception.h"
#include <QtNetwork/QtNetwork>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

PatchKitRemotePatcher::PatchKitRemotePatcher() :
    m_isCancelled(false)
{
}

int PatchKitRemotePatcher::getVersion(const QString& t_patcherSecret)
{
    QString jsonData = downloadString(QString("http://api.patchkit.net/1/apps/%1/versions/latest/id").arg(t_patcherSecret));

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

QString PatchKitRemotePatcher::download(const QString& t_patcherSecret, int t_version)
{
    QStringList contentUrls = getContentUrls(t_patcherSecret, t_version);

    QString filePath("file.zip");

    downloadFile(filePath, contentUrls[0]);

    return filePath;
}

void PatchKitRemotePatcher::cancel()
{
    m_isCancelled = true;

    emit cancelled();
}

QStringList PatchKitRemotePatcher::getContentUrls(const QString& t_patcherSecret, int t_version) const
{
    QString jsonData = downloadString(QString("http://api.patchkit.net/1/apps/%1/versions/%2/content_urls").arg(t_patcherSecret, QString::number(t_version)));

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

    QStringList result;

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

        result.append(jsonValue.toString());
    }

    return result;
}

void PatchKitRemotePatcher::getNetworkReply(const QString& t_urlPath, std::auto_ptr<QNetworkAccessManager>& t_accessManager, std::auto_ptr<QNetworkReply>& t_reply) const
{
    qDebug() << QString("Getting network reply from %1").arg(t_urlPath).toStdString().c_str();
    QUrl url(t_urlPath);
    t_accessManager = std::auto_ptr<QNetworkAccessManager>(new QNetworkAccessManager());
    t_reply = std::auto_ptr<QNetworkReply>(t_accessManager.get()->get( QNetworkRequest(url)));

    QEventLoop readyReadLoop;
    connect(t_reply.get(), &QNetworkReply::readyRead, &readyReadLoop, &QEventLoop::quit);

    connect(this, &PatchKitRemotePatcher::cancelled, &readyReadLoop, &QEventLoop::quit);

    QTimer timeoutTimer;

    connect(&timeoutTimer, &QTimer::timeout, &readyReadLoop, &QEventLoop::quit);

    timeoutTimer.setInterval(downloadTimeoutInSeconds *1000);
    timeoutTimer.setSingleShot(true);
    timeoutTimer.start();

    readyReadLoop.exec();

    if(m_isCancelled)
    {
        throw LauncherCancelledException();
    }

    if(!timeoutTimer.isActive())
    {
        throw LauncherException("Request timeout.");
    }

    if(t_reply.get()->error() != QNetworkReply::NoError)
    {
        throw LauncherException(t_reply.get()->errorString());
    }

    QVariant statusCode = t_reply.get()->attribute(QNetworkRequest::HttpStatusCodeAttribute);

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

QString PatchKitRemotePatcher::downloadString(const QString& t_urlPath) const
{
    std::auto_ptr<QNetworkAccessManager> accessManager;
    std::auto_ptr<QNetworkReply> reply;
    getNetworkReply(t_urlPath, accessManager, reply);

    QString result = QString(reply->readAll());

    qDebug() << result;

    return result;
}

void PatchKitRemotePatcher::downloadFile(const QString& t_filePath, const QString& t_urlPath) const
{
    std::auto_ptr<QNetworkAccessManager> accessManager;
    std::auto_ptr<QNetworkReply> reply;
    getNetworkReply(t_urlPath, accessManager, reply);

    connect(reply.get(), &QNetworkReply::downloadProgress, this, &PatchKitRemotePatcher::downloadProgress);

    QEventLoop finishedLoop;
    connect(reply.get(), &QNetworkReply::finished, &finishedLoop, &QEventLoop::quit);

    connect(this, &PatchKitRemotePatcher::cancelled, &finishedLoop, &QEventLoop::quit);

    finishedLoop.exec();

    if (m_isCancelled)
    {
        throw LauncherCancelledException();
    }

    QFile file(t_filePath);

    if(!file.open(QFile::WriteOnly))
    {
        throw LauncherException("Couldn't open file for download.");
    }

    file.write(reply->readAll());

    file.flush();
    file.close();
}
