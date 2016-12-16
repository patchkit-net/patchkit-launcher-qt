/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "remotepatcherdata.h"

#include "logger.h"
#include "config.h"
#include "timeoutexception.h"
#include "chunkeddownloader.h"

int RemotePatcherData::getVersion(const Data& t_data, CancellationToken t_cancellationToken)
{
    logInfo("Fetching newest patcher version from 1/apps/%1/versions/latest/id", .arg(Logger::adjustSecretForLog(t_data.patcherSecret())));

    QString result = m_api.downloadString(QString("1/apps/%1/versions/latest/id").arg(t_data.patcherSecret()), t_cancellationToken);

    return parseVersionJson(result);
}

QString RemotePatcherData::getPatcherSecret(const Data& t_data, CancellationToken t_cancellationToken)
{
    logInfo("Fetching newest patcher secret from 1/apps/%1", .arg(Logger::adjustSecretForLog(t_data.applicationSecret())));

    QString result = m_api.downloadString(QString("1/apps/%1").arg(t_data.applicationSecret()), t_cancellationToken);

    return parsePatcherSecret(result);
}

void RemotePatcherData::download(const QString& t_downloadPath, const Data& t_data, int t_version, CancellationToken t_cancellationToken)
{
    logInfo("Downloading patcher %1 version", .arg(QString::number(t_version)));

    QStringList contentUrls = getContentUrls(t_data.patcherSecret(), t_version, t_cancellationToken);

    QString contentSummaryPath = QString("1/apps/%1/versions/%2/content_summary").arg(t_data.patcherSecret(), QString::number(t_version));

    logInfo("Downloading content summary from %1", .arg(contentSummaryPath));

    ContentSummary summary = m_api.downloadContentSummary(contentSummaryPath, t_cancellationToken);

    ChunkedDownloader chunkedDownloader(summary, t_cancellationToken);

    connect(&chunkedDownloader, &ChunkedDownloader::downloadProgressChanged, this, &RemotePatcherData::downloadProgressChanged);

    for (int i = 0; i < contentUrls.size(); i++)
    {
        t_cancellationToken.throwIfCancelled();

        logInfo("Downloading from %1", .arg(contentUrls[i]));

        try
        {
            try
            {
                chunkedDownloader.downloadFile(contentUrls[i], t_downloadPath, Config::minConnectionTimeoutMsec);
                return;
            }
            catch (TimeoutException&)
            {
                chunkedDownloader.downloadFile(contentUrls[i], t_downloadPath, Config::maxConnectionTimeoutMsec);
                return;
            }
        }
        catch (CancelledException&)
        {
            throw;
        }
        catch (QException& exception)
        {
            if (QFile::exists(t_downloadPath))
            {
                QFile::remove(t_downloadPath);
            }
            logWarning(exception.what());
        }
        catch (std::runtime_error& err)
        {
            if (QFile::exists(t_downloadPath))
            {
                QFile::remove(t_downloadPath);
            }
            logWarning(QString("STD runtime error: %1").arg(err.what()));
        }
        catch (...)
        {
            if (QFile::exists(t_downloadPath))
            {
                QFile::remove(t_downloadPath);
            }
            logWarning("Unknown exception while downloading patcher.");
        }
    }

    throw std::runtime_error("Unable to download patcher version - " + std::to_string(t_version));
}

QStringList RemotePatcherData::getContentUrls(const QString& t_patcherSecret, int t_version, CancellationToken t_cancellationToken)
{
    logInfo("Fetching patcher content urls from 1/apps/%1/versions/%2/content_urls",.arg(Logger::adjustSecretForLog(t_patcherSecret),
                                                                                         QString::number(t_version)));

    QString result = m_api.downloadString(QString("1/apps/%1/versions/%2/content_urls").arg(t_patcherSecret, QString::number(t_version)), t_cancellationToken);

    return parseContentUrlsJson(result);
}

int RemotePatcherData::parseVersionJson(const QString& t_json)
{
    logInfo("Parsing version from json.");
    logDebug(t_json);

    QJsonDocument jsonDocument = QJsonDocument::fromJson(t_json.toUtf8());

    if (!jsonDocument.isObject())
    {
        throw std::runtime_error("Couldn't read version id from JSON data.");
    }

    QJsonObject jsonObject = jsonDocument.object();

    if (!jsonObject.contains("id"))
    {
        throw std::runtime_error("Couldn't read version id from JSON data.");
    }

    int idValue = jsonObject.value("id").toInt(-1);

    if (idValue == -1)
    {
        throw std::runtime_error("Couldn't read version id from JSON data.");
    }

    return idValue;
}

QString RemotePatcherData::parsePatcherSecret(const QString& t_json)
{
    logInfo("Parsing patcher secret from json.");

    QJsonDocument jsonDocument = QJsonDocument::fromJson(t_json.toUtf8());

    if (!jsonDocument.isObject())
    {
        throw std::runtime_error("Couldn't read patcher secret from JSON data.");
    }

    QJsonObject jsonObject = jsonDocument.object();

    if (!jsonObject.contains("patcher_secret"))
    {
        throw std::runtime_error("Couldn't read patcher secret from JSON data.");
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
        throw std::runtime_error("Couldn't read content urls from JSON data.");
    }

    QJsonArray jsonArray = jsonDocument.array();

    if (jsonArray.size() == 0)
    {
        throw std::runtime_error("Empty content urls.");
    }

    QStringList result;

    for (int i = 0; i < jsonArray.size(); i++)
    {
        if (!jsonArray[i].isObject())
        {
            throw std::runtime_error("Couldn't read content urls from JSON data.");
        }

        QJsonObject jsonObject = jsonArray[i].toObject();

        if (!jsonObject.contains("url"))
        {
            throw std::runtime_error("Couldn't read content urls from JSON data.");
        }

        QJsonValue jsonValue = jsonObject.value("url");

        if (!jsonValue.isString())
        {
            throw std::runtime_error("Couldn't read content urls from JSON data.");
        }

        result.append(jsonValue.toString());
    }

    return result;
}
