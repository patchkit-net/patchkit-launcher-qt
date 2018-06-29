#include "mocks/api.h"

#include "src/contentsummary.h"

MockApi::MockApi(QString t_countryCode, QString t_defaultPatcherSecret,
                 std::function<QStringList (const QString&, int)> t_contentUrlsSelector,
                 std::function<int (const QString&)> t_latestVersionIdSelector,
                 std::function<ContentSummary (const QString&, int)> t_contentSummarySelector,
                 std::function<QString (const QString&)> t_patcherSecretSelector)
    : m_countryCode(t_countryCode)
    , m_defaultPatcherSecret(t_defaultPatcherSecret)
    , m_contentUrlsSelector(t_contentUrlsSelector)
    , m_latestVersionIdSelector(t_latestVersionIdSelector)
    , m_contentSummarySelector(t_contentSummarySelector)
    , m_patcherSecretSelector(t_patcherSecretSelector)
{
}

QString MockApi::getCountryCode(CancellationToken /*t_cancellationToken*/) const
{
    return m_countryCode;
}

QString MockApi::getPatcherSecret(const QString& t_appSecret, CancellationToken /*t_cancellationToken*/) const
{
    return m_patcherSecretSelector(t_appSecret);
}

ContentSummary MockApi::getContentSummary(const QString& t_appSecret, int t_version, CancellationToken /*t_cancellationToken*/) const
{
    return m_contentSummarySelector(t_appSecret, t_version);
}

QString MockApi::getDefaultPatcherSecret(CancellationToken /*t_cancellationToken*/) const
{
    return m_defaultPatcherSecret;
}

int MockApi::getLatestVersionId(const QString& t_appSecret, CancellationToken /*t_cancellationToken*/) const
{
    return m_latestVersionIdSelector(t_appSecret);
}

QStringList MockApi::getContentUrls(const QString& t_appSecret, int t_version, CancellationToken /*t_cancellationToken*/) const
{
    return m_contentUrlsSelector(t_appSecret, t_version);
}
