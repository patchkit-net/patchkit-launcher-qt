/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>

#include "cancellationtoken.h"
#include "contentsummary.h"
#include "iapi.h"

#include "defaultdownloadstrategy.h"
#include "downloader.h"

class Api : public QObject, public IApi
{
    template <typename T>
    struct InternalResponse
    {
        InternalResponse(T data, int statusCode)
            : data(data), statusCode(statusCode)
        {}

        T data;
        int statusCode;
    };

    Q_OBJECT
public:
    CUSTOM_RUNTIME_ERROR(ServerConnectionError)

    explicit Api(
            Downloader::TDataSource t_dataSource,
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
    InternalResponse<QString> geolocate(CancellationToken t_cancellationToken) const;

    InternalResponse<QByteArray> downloadInternal(
            const QString& t_resourceUrl,
            bool t_withGeolocation,
            CancellationToken t_cancellationToken) const;

    Downloader::TDataSource m_dataSource;
};
