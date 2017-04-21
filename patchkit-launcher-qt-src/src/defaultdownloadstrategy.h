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
    DefaultDownloadStrategy(int t_minTimeout, int t_maxTimeout);

    const static int maxStartingDownloadersCount;

protected:
    virtual void init() override;
    virtual void finish() override;
    virtual void proceedInternal() override;
    virtual void stopInternal() override;

    virtual void onDownloaderStarted(Downloader* downloader);
    virtual void onDownloaderFinished(Downloader* downloader);
    virtual void onFirstTimeout();
    virtual void onSecondTimeout();

    void prepareDownloader(Downloader* t_downloader);
    void discardDownloader(Downloader* t_downloader);

    void acceptActiveDownloader(Downloader* t_downloader);
    void discardActiveDownloader(Downloader* t_downloader);

    void reset();

protected slots:
    void onDownloaderStartedInternal();
    void onDownloaderFinishedInternal();
    void onTimeout();

    void printDebugInfo();

private:
    QTimer m_timer;
    int m_timeoutCounter;

    int m_iterator;
    int m_minTimeout;
    int m_maxTimeout;

    QVector<Downloader*> m_activeDownloaders;
    QVector<Downloader*> m_startingDownloaders;
};
