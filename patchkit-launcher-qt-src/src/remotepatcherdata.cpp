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

    ContentSummary summary;

    try
    {
        summary = m_api.downloadContentSummary(contentSummaryPath, t_cancellationToken);
    }
    catch(std::runtime_error& err)
    {
        logWarning(QString("Exception while downloading content summary: %1").arg(err.what()));
    }

    if (summary.isValid())
    {
        logInfo("Beginning chunked download.");
        if (downloadChunked(t_downloadPath, contentUrls, summary, t_cancellationToken))
        {
            return;
        }
    }

    logInfo("Chunked download failed or isn't available, falling back on simple HTTP.");
    if (downloadDirect(t_downloadPath, contentUrls, t_cancellationToken))
    {
        return;
    }

    throw std::runtime_error("Unable to download patcher version - " + std::to_string(t_version));
}

QStringList RemotePatcherData::getContentUrls(const QString& t_patcherSecret, int t_version, CancellationToken t_cancellationToken)
{
    logInfo("Fetching patcher content urls from 1/apps/%1/versions/%2/content_urls",
            .arg(Logger::adjustSecretForLog(t_patcherSecret),QString::number(t_version)));

    QString result = m_api.downloadString(QString("1/apps/%1/versions/%2/content_urls").arg(t_patcherSecret, QString::number(t_version)), t_cancellationToken);

    return parseContentUrlsJson(result);
}

bool RemotePatcherData::downloadWith(Downloader& downloader, const QString& t_downloadPath, const QStringList& t_contentUrls, CancellationToken t_cancellationToken)
{
    const auto cleanup = [t_downloadPath]()
    {
        if (QFile::exists(t_downloadPath))
        {
            QFile::remove(t_downloadPath);
        }
    };

    connect(&downloader, &Downloader::downloadProgressChanged, this, &RemotePatcherData::downloadProgressChanged);

    for (int i = 0; i < t_contentUrls.size(); i++)
    {
        t_cancellationToken.throwIfCancelled();

        logInfo("Downloading from %1", .arg(t_contentUrls[i]));

        try
        {
            try
            {
                downloader.downloadFile(t_contentUrls[i], t_downloadPath, Config::minConnectionTimeoutMsec, t_cancellationToken);
                return true;
            }
            catch (TimeoutException&)
            {
                downloader.downloadFile(t_contentUrls[i], t_downloadPath, Config::maxConnectionTimeoutMsec, t_cancellationToken);
                return true;
            }
        }
        catch (CancelledException&)
        {
            throw;
        }
        catch (StaleDownloadException&)
        {
            logWarning("Download gone stale.");
        }
        catch (QException& exception)
        {
            cleanup();
            logWarning(exception.what());
        }
        catch (std::runtime_error& err)
        {
            cleanup();
            logWarning(QString("STD runtime error: %1").arg(err.what()));
        }
        catch (...)
        {
            cleanup();
            logWarning("Unknown exception while downloading patcher.");
        }
    }

    return false;
}

bool RemotePatcherData::downloadChunked(const QString& t_downloadPath, const QStringList& t_contentUrls, ContentSummary& t_contentSummary, CancellationToken t_cancellationToken)
{
    ChunkedDownloader downloader(t_contentSummary, HashingStrategy::xxHash);

    return downloadWith((Downloader&) downloader, t_downloadPath, t_contentUrls, t_cancellationToken);
}

bool RemotePatcherData::downloadDirect(const QString& t_downloadPath, const QStringList& t_contentUrls, CancellationToken t_cancellationToken)
{
    Downloader downloader;

    return downloadWith(downloader, t_downloadPath, t_contentUrls, t_cancellationToken);
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