/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include "data.h"
#include "downloader.h"
#include "iapi.h"

class QNetworkAccessManager;

class ContentSummary;

class RemotePatcherData : public QObject
{
    Q_OBJECT

public:
    RemotePatcherData(IApi& t_api, QNetworkAccessManager* t_networkAccessManager);

    int getVersion(const Data& t_data, CancellationToken t_cancellationToken);

    QString getPatcherSecret(const Data& t_data, CancellationToken t_cancellationToken);

    void download(QIODevice& t_dataTarget, const Data& t_data, int t_version, CancellationToken t_cancellationToken);

signals:
    void downloadProgressChanged(const long long& t_bytesDownloaded, const long long& t_totalBytes);
    void downloadError(DownloadError t_error);

    void proceed();
    void stop();

private:
    IApi& m_api;

    QStringList getContentUrls(const QString& t_patcherSecret, int t_version, CancellationToken t_cancellationToken);

    bool saveData(QByteArray& t_data, QIODevice& t_dataTarget);

    static int parseVersionJson(const QString& t_json);

    static QString parsePatcherSecret(const QString& t_json);

    static QStringList parseContentUrlsJson(const QString& t_json);

    QNetworkAccessManager* m_networkAccessManager;
};
