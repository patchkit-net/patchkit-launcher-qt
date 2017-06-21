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
    void finish();
    QByteArray data();

public slots:
    void proceed();
    void stop();
    virtual void downloadProgressRelay(const long long& t_bytesDownloaded, const long long& t_totalBytes);

signals:
    void done();
    void error(DownloadError t_error);
    void downloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes);

protected:
    virtual void startInternal() = 0;
    virtual void finishInternal() = 0;
    virtual void proceedInternal() = 0;
    virtual void stopInternal() = 0;

    QByteArray m_data;
    DownloaderOperator* m_operator;
};
