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

    ContentSummary  downloadContentSummary(const QString& t_resourceUrl) override;
    QString         downloadPatcherSecret(const QString& t_resourceUrl) override;
    QString         downloadDefaultPatcherSecret() override;
    int             downloadPatcherVersion(const QString& t_resourceUrl) override;
    QStringList     downloadContentUrls(const QString& t_resourceUrl) override;

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
};
