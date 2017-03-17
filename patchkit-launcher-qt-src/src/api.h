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

class Api : public QObject, public IApi
{
    Q_OBJECT
public:
    explicit Api(Downloader::TDataSource t_dataSource, CancellationToken t_cancellationToken, QObject* parent = nullptr);

    ContentSummary  downloadContentSummary(const QString& t_resourceUrl) override;
    QString         downloadPatcherSecret(const QString& t_resourceUrl) override;
    int             downloadPatcherVersion(const QString& t_resourceUrl) override;
    QStringList     downloadContentUrls(const QString& t_resourceUrl) override;

private:
    CancellationToken m_cancellationToken;
    Downloader::TDataSource m_dataSource;
};
