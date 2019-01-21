/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "api.h"

#include "config.h"
#include "remote/downloading/apiconnectionstrategy.h"

#include <QIODevice>
#include <QBuffer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace remote
{
namespace api
{

Api::Api(QNetworkAccessManager& dataSource)
    : m_dataSource(dataSource)
{
}

Api::~Api()
{
}

QJsonDocument Api::get(const QString& path, CancellationToken cancellationToken) const
{
    qInfo() << "Executing GET " << path;

    ApiConnectionSettings connectionSettings = defaultConnectionSettings();
    ApiConnectionStrategy strategy(connectionSettings, Config::minConnectionTimeoutMsec, Config::maxConnectionTimeoutMsec);

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);

    if (!strategy.execute(m_dataSource, path, buffer, cancellationToken))
    {
        throw ApiConnectionError("Failed to connect to the api.");
    }

    auto data = buffer.data();
    auto doc = QJsonDocument::fromJson(data);

    return doc;
}

ContentSummary Api::getContentSummary(const QString& appSecret, int versionId, CancellationToken cancellationToken) const
{
    qInfo() << "Getting the content summary for app " << appSecret << " version " << versionId;

    auto path = QString("1/apps/%1/versions/%2/content_summary")
            .arg(appSecret).arg(versionId);

    QJsonDocument document = get(path, cancellationToken);

    return ContentSummary(document);
}

QString Api::getPatcherSecret(const QString &appSecret, CancellationToken cancellationToken) const
{
    qInfo() << "Getting the patcher secret for app " << appSecret;
    auto path = QString("1/apps/%1")
            .arg(appSecret);

    QJsonDocument document = get(path, cancellationToken);

    if (!document.isObject())
    {
        throw InvalidFormat("Expected document root to be object");
    }

    if (!document.object().contains("patcher_secret"))
    {
        throw InvalidFormat("Document did not contain the 'patcher_secret' field");
    }

    if (!document.object()["patcher_secret"].isString())
    {
        throw InvalidFormat("patcher_secret was not a string");
    }

    return document.object()["patcher_secret"].toString();
}

QString Api::getDefaultPatcherSecret(CancellationToken cancellationToken) const
{
    qInfo() << "Getting the default patcher secret";
    QString path = "1/system/patchers";

    QJsonDocument document = get(path, cancellationToken);

    if (!document.isArray())
    {
        throw InvalidFormat("Expected document root to be array");
    }

    QJsonArray array = document.array();

    QString platformString = Globals::currentPlatformString();

    for (QJsonValueRef item : array)
    {
        if (item.isObject())
        {
            QJsonObject object = item.toObject();

            if (object.contains(platformString))
            {
                return object[platformString].toString();
            }
        }
    }

    throw InvalidFormat("Failed to resolve patcher for platform " + platformString);
}

int Api::getLatestAppVersion(const QString &appSecret, CancellationToken cancellationToken) const
{
    qInfo() << "Getting the latest app version for app " << appSecret;
    auto path = QString("1/apps/%1/versions/latest/id")
            .arg(appSecret);

    QJsonDocument document = get(path, cancellationToken);

    if (!document.isObject())
    {
        throw InvalidFormat("Expected document root to be object");
    }

    if (!document.object().contains("id"))
    {
        throw InvalidFormat("Document did not contain the 'id' field");
    }

    if (!document.object()["id"].isDouble())
    {

    }

    int id = document.object()["id"].toInt(-1);

    if (id == -1)
    {
        throw InvalidFormat("Value of 'id' field was not an integer");
    }

    return id;
}

QStringList Api::getContentUrls(const QString& appSecret, int versionId, CancellationToken cancellationToken) const
{
    qInfo() << "Getting the content urls for app " << appSecret << " version " << versionId;
    auto path = QString("1/apps/%1/versions/%2/content_urls")
            .arg(appSecret).arg(versionId);

    QJsonDocument document = get(path, cancellationToken);

    QStringList contentUrls;

    if (!document.isArray())
    {
        throw InvalidFormat("Expected document root to be array");
    }

    auto arr = document.array();

    for (auto item : arr)
    {
        if (!item.isObject())
        {
            throw InvalidFormat("Expected array item to be object");
        }

        QJsonObject obj = item.toObject();

        if (!obj.contains("url"))
        {
            throw InvalidFormat("Did not contain the 'url' field");
        }

        contentUrls.push_back(obj["url"].toString());
    }

    return contentUrls;
}

QString Api::tryGetCountryCode(CancellationToken cancellationToken) const
{
    qInfo() << "Getting the country code";
    try
    {
        ApiConnectionSettings connectionSettings(Config::geolocationApiUrl, QStringList());
        ApiConnectionStrategy strategy(connectionSettings, Config::geolocationTimeout, Config::geolocationTimeout);

        QString path = "v1/country";

        QBuffer buffer;

        if (!strategy.execute(m_dataSource, path, buffer, cancellationToken))
        {
            throw ApiConnectionError("Failed to connect to the api.");
        }

        auto document = QJsonDocument::fromBinaryData(buffer.data());

        return document.object()["country"].toString();
    }
    catch (ApiConnectionError&)
    {
        return QString();
    }
}

} // namespace api
} // namespace remote
