/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "remotepatcherdata.h"

#include "logger.h"
#include "config.h"
#include "customexceptions.h"
#include "chunkeddownloader.h"
#include "contentsummary.h"

#include "api.h"

RemotePatcherData::RemotePatcherData(LauncherState& t_launcherState, IApi& t_api, QNetworkAccessManager* t_networkAccessManager)
    : m_networkAccessManager(t_networkAccessManager)
    , m_launcherState(t_launcherState)
    , m_api(t_api)
{
}

int RemotePatcherData::getVersion(const Data& t_data, CancellationToken t_cancellationToken) const
{
    return m_api.getLatestVersionId(t_data.patcherSecret(), t_cancellationToken);
}

QString RemotePatcherData::getPatcherSecret(const Data& t_data, CancellationToken t_cancellationToken)
{
    qInfo() << "Fetching newest patcher secret from 1/apps/" <<  Logger::adjustSecretForLog(t_data.applicationSecret());

    QString patcherSecret = m_api.getPatcherSecret(t_data.applicationSecret(), t_cancellationToken);

    if (patcherSecret.isNull() || patcherSecret.isEmpty())
    {
        qInfo() << "No patcher secret found, attempting to download default patcher secret.";
        patcherSecret = m_api.getDefaultPatcherSecret(t_cancellationToken);
    }

    return patcherSecret;
}

void RemotePatcherData::download(QIODevice& t_dataTarget, const Data& t_data, int t_version, CancellationToken t_cancellationToken)
{
    qInfo("Downloading patcher %d version", t_version);

    QStringList contentUrls = getContentUrls(t_data.patcherSecret(), t_version, t_cancellationToken);

    QString patcherSecret = t_data.patcherSecret();

    qInfo() << "Downloading content summary from 1/apps/"
            << Logger::adjustSecretForLog(patcherSecret)
            << "/versions/"
            << t_version
            << "/content_summary.";

    ContentSummary summary;

    summary = m_api.getContentSummary(patcherSecret, t_version, t_cancellationToken);
    qInfo("Successfully downloaded the content summary.");

    if (summary.isValid())
    {
        ChunkedDownloader downloader(
                    m_networkAccessManager,
                    summary,
                    HashingStrategy::xxHash,
                    t_cancellationToken,
                    m_launcherState,
                    m_api);

        connect(&downloader, &ChunkedDownloader::downloadProgress, this, &RemotePatcherData::downloadProgressChanged);

        QByteArray data = downloader.downloadFile(contentUrls);

        saveData(data, t_dataTarget);

        return;
    }
    else
    {
        qCritical("INVALID CONTENT SUMMARY");
    }

    throw std::runtime_error("Unable to download patcher version - " + std::to_string(t_version));
}

QStringList RemotePatcherData::getContentUrls(const QString& t_patcherSecret, int t_version, CancellationToken t_cancellationToken)
{
    qInfo() << "Fetching patcher content urls from 1/apps/"
            << Logger::adjustSecretForLog(t_patcherSecret)
            << "/versions/"
            << t_version
            << "/content_urls";

    return m_api.getContentUrls(t_patcherSecret, t_version, t_cancellationToken);
}

bool RemotePatcherData::saveData(QByteArray& t_data, QIODevice& t_dataTarget)
{
    if (!t_dataTarget.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open data target for writing.");
        return false;
    }

    qInfo("Saving downloaded data.");

    t_dataTarget.write(t_data);

    t_dataTarget.close();

    return true;
}

int RemotePatcherData::parseVersionJson(const QString& t_json)
{
    qInfo("Parsing version from json.");
    qDebug() << t_json;

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
    qInfo("Parsing patcher secret from json.");

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
    qInfo("Parsing content urls from json.");
    qDebug() << t_json;

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
