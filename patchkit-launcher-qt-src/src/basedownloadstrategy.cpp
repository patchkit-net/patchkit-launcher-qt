/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "basedownloadstrategy.h"

#include "downloaderoperator.h"

#include <QMessageBox>

void BaseDownloadStrategy::start(DownloaderOperator* t_operator)
{
    m_operator = t_operator;

    startInternal();
}

void BaseDownloadStrategy::finish()
{
    finishInternal();
}

QByteArray BaseDownloadStrategy::data()
{
    return m_data;
}

void BaseDownloadStrategy::proceed()
{
    proceedInternal();
}

void BaseDownloadStrategy::stop()
{
    stopInternal();
}

void BaseDownloadStrategy::downloadProgressRelay(const long long& t_bytesDownloaded, const long long& t_totalBytes)
{
    emit downloadProgress(t_bytesDownloaded, t_totalBytes);
}
