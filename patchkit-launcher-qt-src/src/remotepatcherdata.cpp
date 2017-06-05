/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "remotepatcherdata.h"

#include "logger.h"
#include "config.h"
#include "timeoutexception.h"
#include "staledownloadexception.h"
#include "chunkeddownloader.h"
#include "contentsummary.h"

#include "api.h"

RemotePatcherData::RemotePatcherData(Api& t_api, QNetworkAccessManager* t_networkAccessManager)
    : m_api(t_api)
    , m_networkAccessManager(t_networkAccessManager)
{
    connect(&m_api, &Api::downloadError, this, &RemotePatcherData::downloadError);

    connect(this, &RemotePatcherData::proceed, &m_api, &Api::proceed);
    connect(this, &RemotePatcherData::stop, &m_api, &Api::stop);
}

int RemotePatcherData::getVersion(const Data& t_data, CancellationToken t_cancellationToken)
{
    logInfo("Fetching newest patcher version from 1/apps/%1/versions/latest/id", .arg(Logger::adjustSecretForLog(t_data.patcherSecret())));

    QString resourceUrl = QString("1/apps/%1/versions/latest/id").arg(t_data.patcherSecret());

    return m_api.downloadPatcherVersion(resourceUrl);
}

QString RemotePatcherData::getPatcherSecret(const Data& t_data, CancellationToken t_cancellationToken)
{
    logInfo("Fetching newest patcher secret from 1/apps/%1", .arg(Logger::adjustSecretForLog(t_data.applicationSecret())));

    QString resourceUrl = QString("1/apps/%1").arg(t_data.applicationSecret());

    return m_api.downloadPatcherSecret(resourceUrl);
}

void RemotePatcherData::download(QIODevice& t_dataTarget, const Data& t_data, int t_version, CancellationToken t_cancellationToken)
{
    logInfo("Downloading patcher %1 version", .arg(QString::number(t_version)));

    QStringList contentUrls = getContentUrls(t_data.patcherSecret(), t_version, t_cancellationToken);

    QString patcherSecret = t_data.patcherSecret();
    QString version = QString::number(t_version);

    QString contentSummaryPath = QString("1/apps/%1/versions/%2/content_summary").arg(patcherSecret, version);

    logInfo("Downloading content summary from 1/apps/%1/versions/%2/content_summary.",
            .arg(Logger::adjustSecretForLog(patcherSecret), version));

    ContentSummary summary;

    try
    {
        summary = m_api.downloadContentSummary(contentSummaryPath);
        logInfo("Successfully downloaded the content summary.");
    }
    catch(std::runtime_error& err)
    {
        logWarning(QString("Exception while downloading content summary: %1").arg(err.what()));
    }

    if (summary.isValid())
    {
        ChunkedDownloader downloader(m_networkAccessManager, summary, HashingStrategy::xxHash, t_cancellationToken);

        connect(&downloader, &ChunkedDownloader::downloadError, this, &RemotePatcherData::downloadError);
        connect(&downloader, &ChunkedDownloader::downloadProgress, this, &RemotePatcherData::downloadProgressChanged);

        connect(this, &RemotePatcherData::proceed, &downloader, &ChunkedDownloader::proceed);
        connect(this, &RemotePatcherData::stop, &downloader, &ChunkedDownloader::stop);

        QByteArray data = downloader.downloadFile(contentUrls);

        saveData(data, t_dataTarget);

        return;
    }
    else
    {
        logCritical("INVALID CONTENT SUMMARY");
    }

    throw std::runtime_error("Unable to download patcher version - " + std::to_string(t_version));
}

QStringList RemotePatcherData::getContentUrls(const QString& t_patcherSecret, int t_version, CancellationToken t_cancellationToken)
{
    logInfo("Fetching patcher content urls from 1/apps/%1/versions/%2/content_urls",
            .arg(Logger::adjustSecretForLog(t_patcherSecret),QString::number(t_version)));

    QString resourceUrl = QString("1/apps/%1/versions/%2/content_urls").arg(t_patcherSecret, QString::number(t_version));

    return m_api.downloadContentUrls(resourceUrl);
}

bool RemotePatcherData::saveData(QByteArray& t_data, QIODevice& t_dataTarget)
{
    if (!t_dataTarget.open(QIODevice::WriteOnly))
    {
        logWarning("Couldn't open data target for writing.");
        return false;
    }

    logInfo("Saving downloaded data.");

    t_dataTarget.write(t_data);

    t_dataTarget.close();

    return true;
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
