/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QNetworkAccessManager>

#include "cancellation/cancellationtoken.h"
#include "data/contentsummary.h"
#include "customexceptions.h"

#include "iapi.h"

namespace remote
{
namespace api
{
class Api : public IApi
{
public:
    CUSTOM_RUNTIME_ERROR(ApiConnectionError)
    CUSTOM_RUNTIME_ERROR(InvalidFormat)

    Api(QNetworkAccessManager& dataSource);
    ~Api() override;

    virtual ContentSummary getContentSummary(
            const QString& appSecret, int versionId, CancellationToken cancellationToken) const override;

    virtual QString getPatcherSecret(
            const QString& appSecret, CancellationToken cancellationToken) const override;

    virtual QString getDefaultPatcherSecret(
            CancellationToken cancellationToken
            ) const override;

    virtual int getLatestAppVersion(
            const QString& appSecret, CancellationToken cancellationToken) const override;

    virtual QStringList getContentUrls(
            const QString& appSecret, int versionId, CancellationToken cancellationToken) const override;

private:
    QJsonDocument get(const QString& path, CancellationToken cancellationToken) const;
    QString tryGetCountryCode(CancellationToken cancellationToken) const;

    QNetworkAccessManager& m_dataSource;
    QString m_countryCode;
};

} // namespace api
} // namespace remote
