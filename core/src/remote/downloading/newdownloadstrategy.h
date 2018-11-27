#include "cancellation/cancellationtoken.h"

#include <QNetworkAccessManager>
#include <QStringList>
#include <QUrl>
#include <QVector>

#include "remote/api/apiconnectionsetting.h"

#pragma once

///
/// \brief The SimpleDownloadStrategy class
///
/// Attempts to download from each provided url.
///
class SimpleDownloadStrategy
{
public:
    SimpleDownloadStrategy(const ApiConnectionSettings& apiConnectionSettings,
                           int minConnectionTimeout,
                           int maxConnectionTimeout);

    bool execute(
            QNetworkAccessManager& nam,
            QString path,
            QIODevice& target,
            CancellationToken cancellationToken) const;

private:
    ApiConnectionSettings m_apiConnectionSettings;
    int m_minConnectionTimeout;
    int m_maxConnectionTimeout;

    static QByteArray downloadFromUrl(const QUrl& url);
};
