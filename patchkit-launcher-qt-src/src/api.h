/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>

#include "cancellationtoken.h"

#include "contentsummary.h"

#include "iapi.h"
#include "downloader.h"

#include "defaultdownloadstrategy.h"

class Api : public QObject, public IApi
{
    Q_OBJECT
public:
    explicit Api(Downloader::TDataSource t_dataSource, CancellationToken t_cancellationToken, QObject* parent = nullptr);

    ContentSummary  downloadContentSummary(const QString& t_resourceUrl);
    QString         downloadPatcherSecret(const QString& t_resourceUrl);
    QString         downloadDefaultPatcherSecret();
    int             downloadPatcherVersion(const QString& t_resourceUrl);

    QStringList     downloadContentUrls(const QString& t_resourceUrl);
    QStringList     downloadContentUrls(const QString& t_patcherSecret, const QString& t_version);
    QStringList     downloadContentUrls(const QString& t_patcherSecret, int version);

    bool            geolocate();

    const QString&  getCountryCode() const override;

signals:
    void downloadError(DownloadError t_error);

    void proceed();
    void stop();

private slots:
    void downloadErrorRelay(DownloadError t_error);

private:
    QByteArray downloadInternal(const QString& t_resourceUrl);
    DefaultDownloadStrategy m_strategy;
    CancellationToken m_cancellationToken;
    Downloader::TDataSource m_dataSource;

    bool m_didLastDownloadSucceed;

    QString m_countryCode;
};
