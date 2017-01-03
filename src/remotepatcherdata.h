/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include "data.h"
#include "downloader.h"
#include "api.h"

class RemotePatcherData : public QObject
{
    Q_OBJECT

public:
    int getVersion(const Data& t_data, CancellationToken t_cancellationToken);

    QString getPatcherSecret(const Data& t_data, CancellationToken t_cancellationToken);

    void download(const QString& t_downloadPath, const Data& t_data, int t_version, CancellationToken t_cancellationToken);

signals:
    void downloadProgressChanged(const long long& t_bytesDownloaded, const long long& t_totalBytes);

private:
    Api m_api;

    QStringList getContentUrls(const QString& t_patcherSecret, int t_version, CancellationToken t_cancellationToken);

    bool downloadChunked(const QString& t_downloadPath, const QStringList& t_contentUrls, ContentSummary& t_contentSummary, CancellationToken t_cancellationToken);
    bool downloadDirect(const QString& t_downloadPath, const QStringList& t_contentUrls, CancellationToken t_cancellationToken);

    bool downloadWith(Downloader& downloader, const QString& t_downloadPath, const QStringList& t_contentUrls, CancellationToken t_cancellationToken);

    static int parseVersionJson(const QString& t_json);

    static QString parsePatcherSecret(const QString& t_json);

    static QStringList parseContentUrlsJson(const QString& t_json);
};
