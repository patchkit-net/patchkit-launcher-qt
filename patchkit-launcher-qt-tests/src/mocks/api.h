#pragma once

#include "src/iapi.h"

#include <functional>

class MockApi : public IApi
{
public:
    MockApi(QString t_countryCode = QString(), QString t_defaultPatcherSecret = QString(),
            std::function<QStringList(const QString&, int)> t_contentUrlsSelector = {},
            std::function<int(const QString&)> t_latestVersionIdSelector = {},
            std::function<ContentSummary(const QString&, int)> t_contentSummarySelector = {},
            std::function<QString(const QString&)> t_patcherSecretSelector = {});

    QString getCountryCode(
            CancellationToken t_cancellationToken) const override;

    QString getPatcherSecret(
            const QString& t_appSecret, CancellationToken t_cancellationToken) const override;

    ContentSummary getContentSummary(
            const QString& t_appSecret, int t_version, CancellationToken t_cancellationToken) const override;

    QString getDefaultPatcherSecret(
            CancellationToken t_cancellationToken) const override;

    int getLatestVersionId(
            const QString& t_appSecret, CancellationToken t_cancellationToken) const override;

    QStringList getContentUrls(
            const QString& t_appSecret, int t_version, CancellationToken t_cancellationToken) const override;

private:
    QString m_countryCode;
    QString m_defaultPatcherSecret;

    std::function<QStringList(const QString&, int)> m_contentUrlsSelector;
    std::function<int(const QString&)> m_latestVersionIdSelector;
    std::function<ContentSummary(const QString&, int)> m_contentSummarySelector;
    std::function<QString(const QString&)> m_patcherSecretSelector;
};
