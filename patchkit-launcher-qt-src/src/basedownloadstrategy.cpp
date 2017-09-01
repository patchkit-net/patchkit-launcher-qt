/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "basedownloadstrategy.h"

#include "downloaderoperator.h"

#include <QMessageBox>

BaseDownloadStrategy::BaseDownloadStrategy(DownloaderOperator& t_operator, LauncherState& t_state)
    : m_operator(t_operator)
    , m_state(t_state)
{
}

void BaseDownloadStrategy::start()
{
    execute();
}

QByteArray BaseDownloadStrategy::data()
{
    return m_data;
}

void BaseDownloadStrategy::downloadProgressRelay(const long long& t_bytesDownloaded, const long long& t_totalBytes)
{
    emit downloadProgress(t_bytesDownloaded, t_totalBytes);
}
