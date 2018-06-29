/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>

#include "idownloadstrategy.h"
#include "downloader.h"

class DownloaderOperator;

class BaseDownloadStrategy : public QObject, public IDownloadStrategy
{
    Q_OBJECT
public:
    BaseDownloadStrategy(DownloaderOperator& t_operator);

    void start(CancellationToken t_cancellationToken);

    QByteArray data();
    int statusCode();

public slots:
    virtual void downloadProgressRelay(const long long& t_bytesDownloaded, const long long& t_totalBytes);

signals:
    void stop();
    void error(DownloadError t_error);
    void downloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes);

protected:
    void watchProgressOf(Downloader* t_downloader);
    void stopWatchingProgressOf(Downloader* t_downloader);

    int m_statusCode;
    QByteArray m_data;
    DownloaderOperator& m_operator;
};
