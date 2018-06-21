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
        LauncherState& t_state,
        QObject* parent)
    : QObject(parent)
    , m_state(t_state)
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
    qInfo("Download content summary.");
    const QString contentSummaryPath = QString("1/apps/%1/versions/%2/content_summary").arg(t_appSecret, t_version);
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

int Api::downloadPatcherVersion(const QString& t_resourceUrl)
{
    qInfo("Downloading patcher version.");
    QByteArray data;
    data = downloadInternal(t_resourceUrl);

    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isObject())
    {
        throw ServerConnectionError("Couldn't read version id from JSON data.");
    }

    QJsonObject jsonObject = doc.object();

    if (!jsonObject.contains("id"))
    {
        throw ServerConnectionError("Couldn't read version id from JSON data.");
    }

    int idValue = jsonObject.value("id").toInt(-1);

    if (idValue == -1)
    {
        throw ServerConnectionError("Couldn't read version id from JSON data.");
    }

    return idValue;
}

QStringList Api::getContentUrls(
        const QString &t_appSecret, int t_version, CancellationToken t_cancellationToken) const
{
    qInfo("Downloading content urls.");
    const QString resourceUrl = QString("1/api/").arg(t_appSecret, t_version);

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

QString Api::parseGeolocation(const QByteArray& data) const
{
    if (data.isEmpty())
    {
        qWarning("Geolocation failed, reply data was empty.");
        return QString();
    }

    auto jsonDocument = QJsonDocument::fromJson(data);

    if (!jsonDocument.isObject())
    {
        qWarning("Geolocation failed, couldn't create a json document from data.");
        return QString();
    }

    auto root = jsonDocument.object();

    if (!root.contains("country"))
    {
        qWarning("Geolocation failed, no field named country in the root json object.");
        return QString();
    }

    auto countryValue = root.value("country");

    if (!countryValue.isString())
    {
        qWarning("Geolocation failed, country field value was not a string.");
        return QString();
    }

    auto stringValue = countryValue.toString();

    return stringValue;
}

Api::InternalResponse<QString> Api::geolocate()
{
    Downloader downloader(Config::geolocationApiUrl, m_dataSource, m_cancellationToken);
    DownloaderOperator op({&downloader});

    op.startAll();

    auto finishedDownloader = op.waitForAnyToFinish(m_cancellationToken, Config::geolocationTimeout);

    if (!finishedDownloader)
    {
        qWarning("Geolocation download process failed.");
        return InternalResponse<QString>(QString(), -1);
    }

    int replyStatusCode = finishedDownloader->getStatusCode();

    QByteArray data = finishedDownloader->readData();

    return InternalResponse<QString>(parseGeolocation(data), replyStatusCode);
}

Api::InternalResponse<QByteArray> Api::downloadInternal(
        const QString& t_resourceUrl, bool t_withGeolocation, CancellationToken t_cancellationToken)
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

    m_didLastDownloadSucceed = true;

    StringConcatUrlProvider urlProvider(totalUrlBases, t_resourceUrl);
    if (t_withGeolocation && m_countryCode != QString())
    {
        urlProvider.setCountryCode(m_countryCode);
    }

    urlProvider.setIdentifier(Globals::version());

    DownloaderOperator op(m_dataSource, urlProvider, m_cancellationToken);

    DefaultDownloadStrategy strategy(
                op,
                m_state,
                Config::minConnectionTimeoutMsec,
                Config::maxConnectionTimeoutMsec);

    DownloaderOperator::Result result = op.download(strategy, m_cancellationToken);

    if (result.data.size() == 0)
    {
        throw ServerConnectionError("Failed to connect to server.");
    }

    return InternalResponse<QByteArray>(result.data, result.statusCode);
}
