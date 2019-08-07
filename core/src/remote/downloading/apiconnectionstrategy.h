#pragma once

#include "cancellation/cancellationtoken.h"

#include <QNetworkAccessManager>
#include <QStringList>
#include <QUrl>
#include <QVector>

#include "remote/api/apiconnectionsetting.h"
#include "remote/downloading/timeout.h"

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
                           Timeout minConnectionTimeout,
                           Timeout maxConnectionTimeout);

    bool execute(
            QNetworkAccessManager& nam,
            QString path,
            QIODevice& target,
            CancellationToken cancellationToken) const;

private:
    ApiConnectionSettings m_apiConnectionSettings;
    Timeout m_minConnectionTimeout;
    Timeout m_maxConnectionTimeout;

    static QByteArray downloadFromUrl(const QUrl& url);
};

} // namespace api
} // namespace remote
