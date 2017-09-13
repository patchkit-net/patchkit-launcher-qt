/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "api.h"

#include "customexceptions.h"
#include "config.h"

#include "downloaderoperator.h"
#include "defaultdownloadstrategy.h"

#include "contentsummary.h"

#include "logger.h"

Api::Api(
        Downloader::TDataSource t_dataSource,
        CancellationToken t_cancellationToken,
        LauncherState& t_state,
        QObject* parent)
    : QObject(parent)
    , m_cancellationToken(t_cancellationToken)
    , m_state(t_state)
    , m_dataSource(t_dataSource)
{
}

ContentSummary Api::downloadContentSummary(const QString& t_resourceUrl)
{
    qInfo("Download content summary.");
    QByteArray data;
    data = downloadInternal(t_resourceUrl);

    QJsonDocument doc = QJsonDocument::fromJson(data);
    return ContentSummary(doc);
}

QString Api::downloadPatcherSecret(const QString& t_resourceUrl)
{
    qInfo("Downloading patcher secret.");
    QByteArray data;
    data = downloadInternal(t_resourceUrl);

    if (!m_didLastDownloadSucceed)
    {
        throw ContentUnavailableException("Couldn't download patcher secret.");
    }

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

QString Api::downloadDefaultPatcherSecret()
{
    QString resourceUrl = "1/system/patchers";

    auto data = downloadInternal(resourceUrl);

    auto jsonDocument = QJsonDocument::fromJson(data);

    if (!jsonDocument.isArray())
    {
        throw std::runtime_error("Couldn't read default patchers from JSON data.");
    }

    auto jsonArray = jsonDocument.array();

    if (jsonArray.size() == 0)
    {
        throw std::runtime_error("Empty patcher list.");
    }

    std::vector<std::pair<QString, QString>> availableDefaultPatchers;

    for (int i = 0; i < jsonArray.size(); i++)
    {
        if (!jsonArray[i].isObject())
        {
            throw std::runtime_error("Unexpected value in the array.");
        }

        auto object = jsonArray[i].toObject();

        if (!(object.contains("platform") && object.contains("secret")))
        {
            throw std::runtime_error("Unexpected values in array object.");
        }

        if (!object["platform"].isString())
        {
            throw std::runtime_error("Platform value is not string.");
        }

        if (!object["secret"].isString())
        {
            throw std::runtime_error("Secret value is not string.");
        }

        auto defaultPatcher = std::make_pair(object["platform"].toString(), object["secret"].toString());
        availableDefaultPatchers.push_back(defaultPatcher);
    }

    for (auto defaultPatcher : availableDefaultPatchers)
    {
        if (defaultPatcher.first == Globals::toString(Globals::currentPlatform()))
        {
            return defaultPatcher.second;
        }
    }

    throw std::runtime_error("Couldn't resolve a default patcher for current platform.");
    return "";
}

int Api::downloadPatcherVersion(const QString& t_resourceUrl)
{
    qInfo("Downloading patcher version.");
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
    qInfo("Downloading content urls.");

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

QStringList Api::downloadContentUrls(const QString& t_patcherSecret, const QString& t_version)
{
    QString resourceUrl = QString("1/apps/%1/versions/%2/content_urls").arg(t_patcherSecret, t_version);

    return downloadContentUrls(resourceUrl);
}

QStringList Api::downloadContentUrls(const QString& t_patcherSecret, int t_version)
{
    return downloadContentUrls(t_patcherSecret, QString::number(t_version));
}

bool Api::geolocate()
{
    Downloader downloader(Config::geolocationApiUrl, m_dataSource, m_cancellationToken);
    DownloaderOperator op({&downloader});

    op.startAll();

    auto finishedDownloader = op.waitForAnyToFinish(m_cancellationToken, Config::geolocationTimeout);

    if (!finishedDownloader)
    {
        qWarning("Geolocation download process failed.");
        return false;
    }

    int replyStatusCode = finishedDownloader->getStatusCode();

    if (!Downloader::doesStatusCodeIndicateSuccess(replyStatusCode))
    {
        qWarning() << "Geolocation failed, reply status code was " << replyStatusCode;
        return false;
    }

    auto data = finishedDownloader->readData();

    if (data == QByteArray())
    {
        qWarning("Geolocation failed, reply data was empty.");
        return false;
    }

    auto jsonDocument = QJsonDocument::fromJson(data);

    if (!jsonDocument.isObject())
    {
        qWarning("Geolocation failed, couldn't create a json document from data.");
        return false;
    }

    auto root = jsonDocument.object();

    if (!root.contains("country"))
    {
        qWarning("Geolocation failed, no field named country in the root json object.");
        return false;
    }

    auto countryValue = root.value("country");

    if (!countryValue.isString())
    {
        qWarning("Geolocation failed, country field value was not a string.");
        return false;
    }

    auto stringValue = countryValue.toString();
    m_countryCode = stringValue;

    return true;
}

const QString& Api::getCountryCode() const
{
    return m_countryCode;
}

QByteArray Api::downloadInternal(const QString& t_resourceUrl)
{
    QStringList totalUrlBases = QStringList(Config::mainApiUrl);
    totalUrlBases.append(Config::cacheApiUrls);

    m_didLastDownloadSucceed = true;

    StringConcatUrlProvider urlProvider(totalUrlBases, t_resourceUrl);
    if (m_countryCode != QString())
    {
        urlProvider.setCountryCode(m_countryCode);
    }

    DownloaderOperator op(m_dataSource, urlProvider, m_cancellationToken);

    DefaultDownloadStrategy strategy(
                op,
                m_state,
                Config::minConnectionTimeoutMsec,
                Config::maxConnectionTimeoutMsec);

    QByteArray data;
    data = op.download(strategy, m_cancellationToken);

    return data;
}
