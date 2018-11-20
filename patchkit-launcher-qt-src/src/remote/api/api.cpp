/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "api.h"

#include "config.h"
#include "remote/downloading/newdownloadstrategy.h"

#include <QBuffer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Api::Api(QNetworkAccessManager& dataSource)
    : m_dataSource(dataSource)
{
}

Api::~Api()
{
}

QJsonDocument Api::get(const QString &path, CancellationToken cancellationToken) const
{
    ApiConnectionSettings connectionSettings(Config::mainApiUrl, Config::cacheApiUrls);
    SimpleDownloadStrategy strategy(connectionSettings, Config::minConnectionTimeoutMsec, Config::maxConnectionTimeoutMsec);

    QBuffer buffer;

    if (!strategy.execute(m_dataSource, path, buffer, cancellationToken))
    {
        throw ApiConnectionError("Failed to connect to the api.");
    }

    return QJsonDocument::fromBinaryData(buffer.data());
}

ContentSummary Api::getContentSummary(const QString &appSecret, int versionId, CancellationToken cancellationToken) const
{
    auto path = QString("1/apps/%1/versions/%2/content_summary")
            .arg(appSecret, versionId);

    QJsonDocument document = get(path, cancellationToken);

    return ContentSummary(document);
}

QString Api::getPatcherSecret(const QString &appSecret, CancellationToken cancellationToken) const
{
    auto path = QString("1/apps/%1")
            .arg(appSecret);

    QJsonDocument document = get(path, cancellationToken);

    return document.object()["patcher_secret"].toString();
}

QString Api::getDefaultPatcherSecret(CancellationToken cancellationToken) const
{
    QString path = "1/system/patchers";

    QJsonDocument document = get(path, cancellationToken);

    throw 1; // TODO: Implement parsing
}

int Api::getLatestAppVersion(const QString &appSecret, CancellationToken cancellationToken) const
{
    auto path = QString("1/apps/%1/versions/latest/id")
            .arg(appSecret);

    QJsonDocument document = get(path, cancellationToken);

    return document.object()["id"].toInt();
}

QStringList Api::getContentUrls(const QString &appSecret, int versionId, CancellationToken cancellationToken) const
{
    auto path = QString("1/apps/%1/versions/%2/content_urls")
            .arg(appSecret, versionId);

    QJsonDocument document = get(path, cancellationToken);

    QStringList contentUrls;

    auto arr = document.array();

    for (auto item : arr)
    {
        QJsonObject obj = item.toObject();
        contentUrls.push_back(obj["url"].toString());
    }

    return contentUrls;
}

QString Api::tryGetCountryCode(CancellationToken cancellationToken) const
{
    try
    {
        ApiConnectionSettings connectionSettings(Config::geolocationApiUrl, QStringList());
        SimpleDownloadStrategy strategy(connectionSettings, Config::geolocationTimeout, Config::geolocationTimeout);

        QString path = "v1/country";

        QBuffer buffer;

        if (!strategy.execute(m_dataSource, path, buffer, cancellationToken))
        {
            throw ApiConnectionError("Failed to connect to the api.");
        }

        auto document = QJsonDocument::fromBinaryData(buffer.data());

        return document.object()["country"].toString();
    }
    catch (ApiConnectionError)
    {
        return QString();
    }
}
