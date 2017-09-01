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
    void start(DownloaderOperator* t_operator);
    QByteArray data();

public slots:
    virtual void downloadProgressRelay(const long long& t_bytesDownloaded, const long long& t_totalBytes);

signals:
    void stop();
    void error(DownloadError t_error);
    void downloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes);

    QByteArray m_data;
    DownloaderOperator* m_operator;
};
