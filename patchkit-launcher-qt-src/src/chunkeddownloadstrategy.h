/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include "defaultdownloadstrategy.h"

class ChunkedDownloader;

class ChunkedDownloadStrategy : public DefaultDownloadStrategy
{
    Q_OBJECT
public:
    ChunkedDownloadStrategy(int t_minTimeout, int t_maxTimeout, const ChunkedDownloader& t_parent);

public slots:
    virtual void downloadProgressRelay(const long long& t_bytesDownloaded, const long long& t_totalBytes) override;

private:
    const ChunkedDownloader& m_parent;
    void setRanges(int t_from);
};
