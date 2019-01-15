#pragma once

#include "cancellation/cancellationtoken.h"

#include <QNetworkAccessManager>
#include <QStringList>
#include <QUrl>
#include <QVector>

#include "remote/api/apiconnectionsetting.h"

namespace remote
{
namespace api
{
///
/// \brief The ApiConnectionStrategy class
///
/// Attempts to download from each provided url.
///
class ApiConnectionStrategy
{
public:
    ApiConnectionStrategy(const ApiConnectionSettings& apiConnectionSettings,
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

} // namespace api
} // namespace remote
