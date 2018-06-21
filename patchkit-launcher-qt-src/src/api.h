/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>

#include "cancellationtoken.h"
#include "launcherstate.h"

#include "contentsummary.h"

#include "iapi.h"
#include "downloader.h"

#include "defaultdownloadstrategy.h"

class Api : public QObject, public IApi
{
    template <typename T>
    struct InternalResponse
    {
        InternalResponse(T data, int statusCode)
            : data(data), statusCode(statusCode)
        {}

        int statusCode;
        T data;
    };

    Q_OBJECT
public:
    explicit Api(
            Downloader::TDataSource t_dataSource,
            LauncherState& t_state,
            QObject* parent = nullptr);

    QString getCountryCode(CancellationToken t_cancellationToken) const override;

    int     getLatestVersionId(
            const QString& t_appSecret, CancellationToken t_cancellationToken) const override;

    QString getPatcherSecret(
            const QString& t_appSecret, CancellationToken t_cancellationToken) const override;

    ContentSummary getContentSummary(
            const QString& t_appSecret, int t_version, CancellationToken t_cancellationToken) const override;

    QString getDefaultPatcherSecret(
            CancellationToken t_cancellationToken) const override;

    QStringList getContentUrls(
            const QString& t_appSecret, int version, CancellationToken t_cancellationToken) const override;

private:
    QString parseGeolocation(const QByteArray& data) const;

    InternalResponse<QString> geolocate() const;

    InternalResponse<QByteArray> downloadInternal(
            const QString& t_resourceUrl,
            bool t_withGeolocation,
            CancellationToken t_cancellationToken) const;

    LauncherState& m_state;
    Downloader::TDataSource m_dataSource;

    bool m_didLastDownloadSucceed;

    QString m_countryCode;
};
