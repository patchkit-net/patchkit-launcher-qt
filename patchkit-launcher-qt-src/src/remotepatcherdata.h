/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include "data.h"
#include "api.h"

class QNetworkAccessManager;

class ContentSummary;

class RemotePatcherData : public QObject
{
    Q_OBJECT

public:
    RemotePatcherData(IApi& t_api, QNetworkAccessManager* t_networkAccessManager);

    int getVersion(const Data& t_data, CancellationToken t_cancellationToken) const;

    QString getPatcherSecret(const QString& t_applicationSecret, CancellationToken t_cancellationToken) const;

    void download(QIODevice& t_dataTarget, const Data& t_data, int t_version, CancellationToken t_cancellationToken);

signals:
    void downloadProgressChanged(const long long& t_bytesDownloaded, const long long& t_totalBytes);
    void downloadError(DownloadError t_error);

private:
    QNetworkAccessManager* m_networkAccessManager;
    IApi& m_api;

    bool saveData(QByteArray& t_data, QIODevice& t_dataTarget);

};
