/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "basedownloadstrategy.h"

#include "downloaderoperator.h"

#include <QMessageBox>

BaseDownloadStrategy::BaseDownloadStrategy(DownloaderOperator& t_operator)
    : m_operator(t_operator)
{
}

void BaseDownloadStrategy::start(CancellationToken t_cancellationToken)
{
    execute(t_cancellationToken);
}

QByteArray BaseDownloadStrategy::data()
{
    return m_data;
}

int BaseDownloadStrategy::statusCode()
{
    return m_statusCode;
}

void BaseDownloadStrategy::downloadProgressRelay(const long long& t_bytesDownloaded, const long long& t_totalBytes)
{
    emit downloadProgress(t_bytesDownloaded, t_totalBytes);
}

void BaseDownloadStrategy::watchProgressOf(Downloader* t_downloader)
{
    connect(t_downloader, &Downloader::progressChanged,
            this, &BaseDownloadStrategy::downloadProgressRelay);
}

void BaseDownloadStrategy::stopWatchingProgressOf(Downloader* t_downloader)
{
    disconnect(t_downloader, &Downloader::progressChanged,
               this, &BaseDownloadStrategy::downloadProgressRelay);
}
