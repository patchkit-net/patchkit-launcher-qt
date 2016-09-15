/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "remotepatcherdata.h"

#include "logger.h"
#include "config.h"
#include "timeoutexception.h"

int RemotePatcherData::getVersion(const Data& t_data, CancellationToken t_cancellationToken)
{
    logInfo("Fetching newest patcher version from http://api.patchkit.net/1/apps/%1/versions/latest/id", .arg(Logger::adjustSecretForLog(t_data.patcherSecret())));

    QString result = m_api.downloadString(QString("http://api.patchkit.net/1/apps/%1/versions/latest/id").arg(t_data.patcherSecret()), t_cancellationToken);

    return parseVersionJson(result);
}

QString RemotePatcherData::getPatcherSecret(const Data& t_data, CancellationToken t_cancellationToken)
{
    logInfo("Fetching newest patcher secret from http://api.patchkit.net/1/apps/%1", .arg(Logger::adjustSecretForLog(t_data.applicationSecret())));

    QString result = m_api.downloadString(QString("http://api.patchkit.net/1/apps/%1").arg(t_data.applicationSecret()), t_cancellationToken);

    return parsePatcherSecret(result);
}

QString RemotePatcherData::download(const Data& t_data, int t_version, CancellationToken t_cancellationToken)
{
    logInfo("Downloading patcher %1 version", .arg(QString::number(t_version)));

    QStringList contentUrls = getContentUrls(t_data.patcherSecret(), t_version, t_cancellationToken);

    Downloader downloader;
    connect(&downloader, &Downloader::downloadProgressChanged, this, &RemotePatcherData::downloadProgressChanged);

    for (int i = 0; i < contentUrls.size(); i++)
    {
        t_cancellationToken.throwIfCancelled();

        QString filePath = QString("file-%1.zip").arg(QString::number(i));

        logInfo("Downloading from %1", .arg(contentUrls[i]));

        try
        {
            try
            {
                downloader.downloadFile(contentUrls[i], filePath, Config::minConnectionTimeoutMsec, t_cancellationToken);
            }
            catch(TimeoutException&)
            {
                downloader.downloadFile(contentUrls[i], filePath, Config::maxConnectionTimeoutMsec, t_cancellationToken);
            }
        }
        catch (CancelledException&)
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

    throw Exception(QString("Unable to download patcher %1 version").arg(QString::number(t_version)));
}

QStringList RemotePatcherData::getContentUrls(const QString& t_patcherSecret, int t_version, CancellationToken t_cancellationToken)
{
    logInfo("Fetching patcher content urls from http://api.patchkit.net/1/apps/%1/versions/%2/content_urls",.arg(Logger::adjustSecretForLog(t_patcherSecret),
        QString::number(t_version)));

    QString result = m_api.downloadString(QString("http://api.patchkit.net/1/apps/%1/versions/%2/content_urls").arg(t_patcherSecret, QString::number(t_version)), t_cancellationToken);

    return parseContentUrlsJson(result);
}

int RemotePatcherData::parseVersionJson(const QString& t_json)
{
    logInfo("Parsing version from json.");
    logDebug(t_json);

    QJsonDocument jsonDocument = QJsonDocument::fromJson(t_json.toUtf8());

    if (!jsonDocument.isObject())
    {
        throw Exception("Couldn't read version id from JSON data.");
    }

    QJsonObject jsonObject = jsonDocument.object();

    if (!jsonObject.contains("id"))
    {
        throw Exception("Couldn't read version id from JSON data.");
    }

    int idValue = jsonObject.value("id").toInt(-1);

    if (idValue == -1)
    {
        throw Exception(QString("Couldn't read version id from JSON data."));
    }

    return idValue;
}

QString RemotePatcherData::parsePatcherSecret(const QString& t_json)
{
    logInfo("Parsing patcher secret from json.");

    QJsonDocument jsonDocument = QJsonDocument::fromJson(t_json.toUtf8());

    if (!jsonDocument.isObject())
    {
        throw Exception("Couldn't read patcher secret from JSON data.");
    }

    QJsonObject jsonObject = jsonDocument.object();

    if (!jsonObject.contains("patcher_secret"))
    {
        throw Exception("Couldn't read patcher secret from JSON data.");
    }

    return jsonObject.value("patcher_secret").toString();
}

QStringList RemotePatcherData::parseContentUrlsJson(const QString& t_json)
{
    logInfo("Parsing content urls from json.");
    logDebug(t_json);

    QJsonDocument jsonDocument = QJsonDocument::fromJson(t_json.toUtf8());

    if (!jsonDocument.isArray())
    {
        throw Exception("Couldn't read content urls from JSON data.");
    }

    QJsonArray jsonArray = jsonDocument.array();

    if (jsonArray.size() == 0)
    {
        throw Exception("Empty content urls.");
    }

    QStringList result;

    for (int i = 0; i < jsonArray.size(); i++)
    {
        if (!jsonArray[i].isObject())
        {
            throw Exception("Couldn't read content urls from JSON data.");
        }

        QJsonObject jsonObject = jsonArray[i].toObject();

        if (!jsonObject.contains("url"))
        {
            throw Exception("Couldn't read content urls from JSON data.");
        }

        QJsonValue jsonValue = jsonObject.value("url");

        if (!jsonValue.isString())
        {
            throw Exception("Couldn't read content urls from JSON data.");
        }

        result.append(jsonValue.toString());
    }

    return result;
}
