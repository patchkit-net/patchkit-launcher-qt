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

}

const QStringList &Api::getApiUrls() const
{

}

ContentSummary Api::downloadContentSummary(IApi::ApiOperationCallback cb, bool *ok) const
{
}

int Api::downloadPatcherVersion(IApi::ApiOperationCallback cb, bool *ok) const
{

}

QString Api::downloadPatcherSecret(IApi::ApiOperationCallback cb, bool *ok) const
{

}

QStringList Api::downloadContentUrls(IApi::ApiOperationCallback cb, bool *ok) const
{

}
