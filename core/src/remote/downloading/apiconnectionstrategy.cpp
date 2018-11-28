#include "apiconnectionstrategy.h"

#include <QtConcurrent/QtConcurrent>

#include <QNetworkReply>

#include "downloadingabstractions.h"

ApiConnectionStrategy::ApiConnectionStrategy(const ApiConnectionSettings& apiConnectionSettings,
                                               int minConnectionTimeout,
                                               int maxConnectionTimeout)
    : m_apiConnectionSettings(apiConnectionSettings)
    , m_minConnectionTimeout(minConnectionTimeout)
    , m_maxConnectionTimeout(maxConnectionTimeout)
{
}

bool ApiConnectionStrategy::execute(
        QNetworkAccessManager &nam,
        QString path,
        QIODevice &target,
        CancellationToken cancellationToken) const
{
    using namespace downloading::abstractions;

    int tryCount = 0;
    bool retry = true;

    do
    {
        int timeout = tryCount == 0 ? m_minConnectionTimeout : m_maxConnectionTimeout;

        if (tryDownload(
                    nam, QString("%1/%2").arg(m_apiConnectionSettings.mainApiUrl, path),
                    target, timeout, cancellationToken))
        {
            return true;
        }

        for (auto cacheServer : m_apiConnectionSettings.cacheApiUrls)
        {
            if (tryDownload(
                        nam, QString("%1/%2").arg(cacheServer, path),
                        target, timeout, cancellationToken))
            {
                return true;
            }
        }

        tryCount++;
        if (tryCount >= 2)
        {
            retry = false;
        }

    } while (retry);

    return false;
}
