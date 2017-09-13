/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include "basedownloadstrategy.h"

class DefaultDownloadStrategy : public BaseDownloadStrategy
{
    Q_OBJECT
public:
    DefaultDownloadStrategy(
            DownloaderOperator& t_operator,
            LauncherState& t_state,
            int t_minTimeout, int t_maxTimeout);

    const static int maxStartingDownloadersCount;

protected:
    virtual void execute(CancellationToken t_cancellationToken);

    virtual bool processFinishedDownloader(Downloader* t_downloader);
    virtual void onConnectionIssues(CancellationToken t_cancellationToken);

private:
    int m_timeoutCounter;

    unsigned int    m_iterator;
    int             m_minTimeout;
    int             m_maxTimeout;
};
