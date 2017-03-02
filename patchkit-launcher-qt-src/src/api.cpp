/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "api.h"

#include "downloader.h"
#include "timeoutexception.h"
#include "config.h"

Api::Api(CancellationToken t_cancellationToken)
    : m_cancellationToken(t_cancellationToken)
    , m_mainApiUrl(Config::mainApiUrl)
    , m_cacheApiUrls(Config::cacheApiUrls)
    , m_minTimeout(Config::minConnectionTimeoutMsec)
    , m_maxTimeout(Config::maxConnectionTimeoutMsec)
{
}

Api::Api(QString t_mainApiUrl, QStringList t_cacheApiUrls, int t_minTimeout, int t_maxTimeout, CancellationToken t_cancellationToken)
    : m_cancellationToken(t_cancellationToken)
    , m_mainApiUrl(t_mainApiUrl)
    , m_cacheApiUrls(t_cacheApiUrls)
    , m_minTimeout(t_minTimeout)
    , m_maxTimeout(t_maxTimeout)
{
}

const QString &Api::getMainApiUrl() const
{
    return m_mainApiUrl;
}

const QStringList &Api::getApiUrls() const
{
    return m_cacheApiUrls;
}

ContentSummary Api::downloadContentSummary(IApi::ApiOperationCallback cb, bool* ok) const
{
}

int Api::downloadPatcherVersion(IApi::ApiOperationCallback cb, bool* ok) const
{

}

QString Api::downloadPatcherSecret(IApi::ApiOperationCallback cb, bool* ok) const
{

}

QStringList Api::downloadContentUrls(IApi::ApiOperationCallback cb, bool* ok) const
{

}

QString Api::downloadRawData(const QString& t_resourceUrl, IApi::ApiOperationCallback cb, bool* t_ok)
{
    auto notify_status = [t_ok](bool val)
    {
        if (t_ok)
        {
            *t_ok = val;
        }
    };

    QString rawData;
    bool ok = false;
    ApiOperationStatus status(0, m_mainApiUrl, m_cacheApiUrls.at(1), m_minTimeout, m_minTimeout);

    // Try main url
    rawData = downloadRawDataInternal(m_mainApiUrl + t_resourceUrl, m_minTimeout, &ok);

    if (ok)
    {
        notify_status(true);
        return rawData;
    }
    else
    {
        if (cb && !cb(this, status))
        {
            notify_status(false);
            return QString();
        }
    }

    // Cache urls
    for (int i = 0; i < m_cacheApiUrls.size(); i++)
    {
        downloadRawDataInternal(m_cacheApiUrls.at(i), m_minTimeout, &ok);
    }
}

QString Api::downloadRawDataInternal(const QString& t_url, int t_timeout, bool *ok)
{
    QNetworkAccessManager networkAccessManager;
    Downloader downloader(&networkAccessManager, m_cancellationToken);

    QString rawData;
    int statusCode = -1;
    rawData = downloader.downloadString(t_url, t_timeout, statusCode);

    if (Downloader::doesStatusCodeIndicateSuccess(statusCode))
    {
        if (ok)
        {
            *ok = true;
        }

        return rawData;
    }
    else
    {
        if (ok)
        {
            *ok = false;
        }

        return QString();
    }
}
