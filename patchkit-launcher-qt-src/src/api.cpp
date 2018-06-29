/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "api.h"

#include "customexceptions.h"
#include "config.h"

#include "downloaderoperator.h"
#include "defaultdownloadstrategy.h"

#include "remoteappdata.h"

#include "contentsummary.h"

#include "logger.h"

#include <QProcessEnvironment>

Api::Api(
        Downloader::TDataSource t_dataSource,
        QObject* parent)
    : QObject(parent)
    , m_dataSource(t_dataSource)
{
}

int Api::getLatestVersionId(const QString& t_appSecret, CancellationToken t_cancellationToken) const
{
    QString resourceUrl = QString("1/apps/%1/versions/latest/id").arg(t_appSecret);

    auto result = downloadInternal(resourceUrl, true, t_cancellationToken);

    return RemoteAppData::parseVersion(result.data);
}

ContentSummary Api::getContentSummary(const QString& t_appSecret, int t_version, CancellationToken t_cancellationToken) const
{
    qInfo("Downloading content summary.");
    const QString contentSummaryPath = QString("1/apps/%1/versions/%2/content_summary").arg(t_appSecret).arg(t_version);
    auto result = downloadInternal(contentSummaryPath, true, t_cancellationToken);

    QJsonDocument doc = QJsonDocument::fromJson(result.data);
    return ContentSummary(doc);
}

QString Api::getPatcherSecret(const QString& t_appSecret, CancellationToken t_cancellationToken) const
{
    qInfo("Downloading patcher secret.");
    const QString resourceUrl = QString("1/apps/%1").arg(t_appSecret);

    auto result = downloadInternal(resourceUrl, true, t_cancellationToken);

    return RemoteAppData::parsePatcherSecret(result.data);
}

QString Api::getDefaultPatcherSecret(CancellationToken t_cancellationToken) const
{
    QString resourceUrl = "1/system/patchers";

    auto result = downloadInternal(resourceUrl, true, t_cancellationToken);

    return RemoteAppData::parseDefaultPatchers(result.data);
}

QStringList Api::getContentUrls(
        const QString &t_appSecret, int t_version, CancellationToken t_cancellationToken) const
{
    qInfo("Downloading content urls.");
    const QString resourceUrl = QString("1/apps/%1/versions/%2/content_urls").arg(t_appSecret).arg(t_version);

    auto result = downloadInternal(resourceUrl, true, t_cancellationToken);

    QJsonDocument jsonDocument = QJsonDocument::fromJson(result.data);

    if (!jsonDocument.isArray())
    {
        throw std::runtime_error("Couldn't read content urls from JSON data.");
    }

    QJsonArray jsonArray = jsonDocument.array();

    if (jsonArray.size() == 0)
    {
        throw std::runtime_error("Empty content urls.");
    }

    QStringList contentUrls;

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

        contentUrls.append(jsonValue.toString());
    }

    return contentUrls;
}

QString Api::getCountryCode(CancellationToken t_cancellationToken) const
{
    auto result = geolocate(t_cancellationToken);
    return result.data;
}

Api::InternalResponse<QString> Api::geolocate(CancellationToken t_cancellationToken) const
{
    Downloader downloader(Config::geolocationApiUrl, m_dataSource, t_cancellationToken);
    DownloaderOperator op({&downloader});

    op.startAll();

    auto finishedDownloader = op.waitForAnyToFinish(t_cancellationToken, Config::geolocationTimeout);

    if (!finishedDownloader)
    {
        qWarning("Geolocation download process failed.");
        return InternalResponse<QString>(QString(), -1);
    }

    int replyStatusCode = finishedDownloader->getStatusCode();

    QByteArray data = finishedDownloader->readData();

    return InternalResponse<QString>(RemoteAppData::tryParseGeolocation(data, QString()), replyStatusCode);
}

Api::InternalResponse<QByteArray> Api::downloadInternal(
        const QString& t_resourceUrl, bool t_withGeolocation, CancellationToken t_cancellationToken) const
{
    auto environment = QProcessEnvironment::systemEnvironment();

    QStringList totalUrlBases;
    if (environment.contains(Config::apiUrlOverrideEnvironmentVariableName))
    {
        totalUrlBases.append(environment.value(Config::apiUrlOverrideEnvironmentVariableName));
    }
    else
    {
        totalUrlBases.append(Config::mainApiUrl);
    }

    if (environment.contains(Config::apiCacheUrlOverrideEnvironmentVariableName))
    {
        totalUrlBases.append(environment.value(Config::apiCacheUrlOverrideEnvironmentVariableName));
    }
    else
    {
        totalUrlBases.append(Config::cacheApiUrls);
    }

    StringConcatUrlProvider urlProvider(totalUrlBases, t_resourceUrl);
    if (t_withGeolocation)
    {
        auto countryCode = getCountryCode(t_cancellationToken);
        if (countryCode != QString())
        {
            urlProvider.setCountryCode(countryCode);
        }
    }

    urlProvider.setIdentifier(Globals::version());

    DownloaderOperator op(m_dataSource, urlProvider, t_cancellationToken);

    DefaultDownloadStrategy strategy(
                op,
                Config::minConnectionTimeoutMsec,
                Config::maxConnectionTimeoutMsec);

    DownloaderOperator::Result result = op.download(strategy, t_cancellationToken);

    if (result.data.size() == 0)
    {
        throw ServerConnectionError("Failed to connect to server.");
    }

    return InternalResponse<QByteArray>(result.data, result.statusCode);
}
