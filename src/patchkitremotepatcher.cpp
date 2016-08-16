#include <QtNetwork/QtNetwork>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include "patchkitremotepatcher.h"
#include "launcherexception.h"
#include "launchercancelledexception.h"
#include "launcherlog.h"
#include <memory>

PatchKitRemotePatcher::PatchKitRemotePatcher() :
    m_isCancelled(false)
{
}

int PatchKitRemotePatcher::getVersion(const LauncherData& t_data)
{
    logInfo("Fetching newest patcher version from http://api.patchkit.net/1/apps/%1/versions/latest/id", .arg(safeSecretLog(t_data.patcherSecret())));

    QString json = downloadString(QString("http://api.patchkit.net/1/apps/%1/versions/latest/id").arg(t_data.patcherSecret()));

    return parseVersionJson(json);
}

QString PatchKitRemotePatcher::download(const LauncherData& t_data, int t_version)
{
    logInfo("Downloading patcher %1 version", .arg(QString::number(t_version)));

    QStringList contentUrls = getContentUrls(t_data.patcherSecret(), t_version);

    for (int i = 0; i < contentUrls.size(); i++)
    {
        if(m_isCancelled)
        {
            throw LauncherCancelledException();
        }

        QString filePath = QString("file-%1.zip").arg(QString::number(i));

        logInfo("Downloading from %1", .arg(contentUrls[i]));

        try
        {
            downloadFile(filePath, contentUrls[i]);
        }
        catch(LauncherCancelledException&)
        {
            throw;
        }
        catch (QException& exception)
        {
            if (QFile::exists(filePath))
            {
                QFile::remove(filePath);
            }
            logWarning(exception.what());
            continue;
        }
        catch (...)
        {
            if (QFile::exists(filePath))
            {
                QFile::remove(filePath);
            }
            logWarning("Unknown exception while downloading patcher.");
            continue;
        }

        return filePath;
    }

    throw LauncherException(QString("Unable to download patcher %1 version").arg(QString::number(t_version)));
}

void PatchKitRemotePatcher::cancel()
{
    logInfo("Cancelling remote patcher operations.");

    m_isCancelled = true;

    emit cancelled();
}

QStringList PatchKitRemotePatcher::getContentUrls(const QString& t_patcherSecret, int t_version) const
{
    logInfo("Fetching patcher content urls from http://api.patchkit.net/1/apps/%1/versions/%2/content_urls",.arg(safeSecretLog(t_patcherSecret),
        QString::number(t_version)));

    QString json = downloadString(QString("http://api.patchkit.net/1/apps/%1/versions/%2/content_urls").arg(t_patcherSecret, QString::number(t_version)));

    return parseContentUrlsJson(json);
}

int PatchKitRemotePatcher::parseVersionJson(const QString& t_json)
{
    logInfo("Parsing version from json.");
    logDebug(t_json);

    QJsonDocument jsonDocument = QJsonDocument::fromJson(t_json.toUtf8());

    if (!jsonDocument.isObject())
    {
        throw LauncherException("Couldn't read version id from JSON data.");
    }

    QJsonObject jsonObject = jsonDocument.object();

    if (!jsonObject.contains("id"))
    {
        throw LauncherException("Couldn't read version id from JSON data.");
    }

    int idValue = jsonObject.value("id").toInt(-1);

    if (idValue == -1)
    {
        throw LauncherException(QString("Couldn't read version id from JSON data."));
    }

    return idValue;
}

QStringList PatchKitRemotePatcher::parseContentUrlsJson(const QString& t_json)
{
    logInfo("Parsing content urls from json.");
    logDebug(t_json);

    QJsonDocument jsonDocument = QJsonDocument::fromJson(t_json.toUtf8());

    if (!jsonDocument.isArray())
    {
        throw LauncherException("Couldn't read content urls from JSON data.");
    }

    QJsonArray jsonArray = jsonDocument.array();

    if (jsonArray.size() == 0)
    {
        throw LauncherException("Empty content urls.");
    }

    QStringList result;

    for (int i = 0; i < jsonArray.size(); i++)
    {
        if (!jsonArray[i].isObject())
        {
            throw LauncherException("Couldn't read content urls from JSON data.");
        }

        QJsonObject jsonObject = jsonArray[i].toObject();

        if (!jsonObject.contains("url"))
        {
            throw LauncherException("Couldn't read content urls from JSON data.");
        }

        QJsonValue jsonValue = jsonObject.value("url");

        if (!jsonValue.isString())
        {
            throw LauncherException("Couldn't read content urls from JSON data.");
        }

        result.append(jsonValue.toString());
    }

    return result;
}

QString PatchKitRemotePatcher::downloadString(const QString& t_urlPath) const
{
    std::shared_ptr<QNetworkAccessManager> accessManager;
    std::shared_ptr<QNetworkReply> reply;
    getNetworkReply(t_urlPath, accessManager, reply);

    QString result = QString(reply->readAll());

    return result;
}

void PatchKitRemotePatcher::downloadFile(const QString& t_filePath, const QString& t_urlPath) const
{
    std::shared_ptr<QNetworkAccessManager> accessManager;
    std::shared_ptr<QNetworkReply> reply;
    getNetworkReply(t_urlPath, accessManager, reply);

    waitForFileDownload(reply);
    writeDownloadedReplyToFile(reply, t_filePath);
}

void PatchKitRemotePatcher::waitForFileDownload(std::shared_ptr<QNetworkReply>& t_reply) const
{
    logInfo("Waiting for file download.");

    connect(t_reply.get(), &QNetworkReply::downloadProgress, this, &PatchKitRemotePatcher::downloadProgressChanged);

    QEventLoop finishedLoop;

    connect(t_reply.get(), &QNetworkReply::finished, &finishedLoop, &QEventLoop::quit);

    connect(this, &PatchKitRemotePatcher::cancelled, &finishedLoop, &QEventLoop::quit);

    finishedLoop.exec();

    disconnect(t_reply.get(), &QNetworkReply::downloadProgress, this, &PatchKitRemotePatcher::downloadProgressChanged);

    if (m_isCancelled)
    {
        throw LauncherCancelledException();
    }
}

void PatchKitRemotePatcher::writeDownloadedReplyToFile(std::shared_ptr<QNetworkReply>& t_reply, const QString& t_filePath)
{
    logInfo("Writing downloaded data to file - %1", .arg(t_filePath));

    QFile file(t_filePath);

    if (!file.open(QFile::WriteOnly))
    {
        throw LauncherException("Couldn't open file for download.");
    }

    qint64 bufferSize = 4096;
    std::unique_ptr<char> buffer(new char[bufferSize]);

    while (!t_reply->atEnd())
    {
        qint64 readSize = t_reply->read(buffer.get(), bufferSize);
        if (readSize > 0)
        {
            file.write(buffer.get(), readSize);
        }
    }

    file.close();
}

void PatchKitRemotePatcher::getNetworkReply(const QString& t_urlPath, std::shared_ptr<QNetworkAccessManager>& t_accessManager, std::shared_ptr<QNetworkReply>& t_reply) const
{
    logInfo("Getting network reply.");

    QUrl url(t_urlPath);
    t_accessManager = std::make_shared<QNetworkAccessManager>();
    t_reply = std::shared_ptr<QNetworkReply>(t_accessManager.get()->get(QNetworkRequest(url)));

    waitForNetworkReply(t_reply);

    validateNetworkReply(t_reply);
}

void PatchKitRemotePatcher::waitForNetworkReply(std::shared_ptr<QNetworkReply>& t_reply) const
{
    logInfo("Waiting for network reply to be ready.");

    QEventLoop readyReadLoop;
    connect(t_reply.get(), &QNetworkReply::readyRead, &readyReadLoop, &QEventLoop::quit);

    connect(this, &PatchKitRemotePatcher::cancelled, &readyReadLoop, &QEventLoop::quit);

    QTimer timeoutTimer;

    connect(&timeoutTimer, &QTimer::timeout, &readyReadLoop, &QEventLoop::quit);

    timeoutTimer.setInterval(downloadTimeoutInSeconds * 1000);
    timeoutTimer.setSingleShot(true);
    timeoutTimer.start();

    readyReadLoop.exec();

    if (m_isCancelled)
    {
        throw LauncherCancelledException();
    }

    if (!timeoutTimer.isActive())
    {
        throw LauncherException("Request timeout");
    }
}

void PatchKitRemotePatcher::validateNetworkReply(std::shared_ptr<QNetworkReply>& t_reply)
{
    logInfo("Validating network reply.");

    if (t_reply.get()->error() != QNetworkReply::NoError)
    {
        throw LauncherException(t_reply.get()->errorString());
    }

    QVariant statusCode = t_reply.get()->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if (!statusCode.isValid())
    {
        throw LauncherException("Couldn't read HTTP status code from reply.");
    }

    int statusCodeValue = statusCode.toInt();

    if (statusCodeValue != 200)
    {
        throw LauncherException(QString("Invaild reply HTTP status code - %1").arg(statusCodeValue));
    }

    logDebug("Reply status code - %1", .arg(statusCodeValue));
}
