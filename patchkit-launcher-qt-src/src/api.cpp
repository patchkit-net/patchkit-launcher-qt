/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "api.h"

#include "timeoutexception.h"
#include "config.h"

#include "downloaderoperator.h"
#include "defaultdownloadstrategy.h"

#include "contentsummary.h"

Api::Api(Downloader::TDataSource t_dataSource, CancellationToken t_cancellationToken, QObject* parent)
    : QObject(parent)
    , m_cancellationToken(t_cancellationToken)
    , m_dataSource(t_dataSource)
{
}

ContentSummary Api::downloadContentSummary(const QString& t_resourceUrl)
{
    QByteArray data;
    data = downloadInternal(t_resourceUrl);

    QJsonDocument doc = QJsonDocument::fromJson(data);
    return ContentSummary(doc);
}

QString Api::downloadPatcherSecret(const QString& t_resourceUrl)
{
    QByteArray data;
    data = downloadInternal(t_resourceUrl);

    QJsonDocument jsonDocument = QJsonDocument::fromJson(data);

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

int Api::downloadPatcherVersion(const QString& t_resourceUrl)
{
    QByteArray data;
    data = downloadInternal(t_resourceUrl);
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isObject())
    {
        throw std::runtime_error("Couldn't read version id from JSON data.");
    }

    QJsonObject jsonObject = doc.object();

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

QStringList Api::downloadContentUrls(const QString& t_resourceUrl)
{
    QByteArray data;
    data = downloadInternal(t_resourceUrl);

    QJsonDocument jsonDocument = QJsonDocument::fromJson(data);

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

QByteArray Api::downloadInternal(const QString& t_resourceUrl)
{
    QStringList totalUrlBases = QStringList(Config::mainApiUrl);
    totalUrlBases.append(Config::cacheApiUrls);

    StringConcatUrlProvider urlProvider(totalUrlBases, t_resourceUrl);

    DefaultDownloadStrategy strategy(Config::minConnectionTimeoutMsec, Config::maxConnectionTimeoutMsec);

    DownloaderOperator op(m_dataSource, urlProvider, m_cancellationToken);
    connect(&strategy, &DefaultDownloadStrategy::error, this, &Api::downloadError);

    connect(this, &Api::proceed, &strategy, &BaseDownloadStrategy::proceed);
    connect(this, &Api::stop, &strategy, &BaseDownloadStrategy::stop);

    QByteArray data;
    data = op.download(&strategy);

    return data;
}
