#ifndef DEFAULTDOWNLOADSTRATEGY_H
#define DEFAULTDOWNLOADSTRATEGY_H

#include "basedownloadstrategy.h"

class DefaultDownloadStrategy : public BaseDownloadStrategy
{
    Q_OBJECT
public:
    DefaultDownloadStrategy(int t_minTimeout, int t_maxTimeout);

protected:
    virtual void init() override;
    virtual void finish() override;
    virtual void proceedInternal() override;
    virtual void stopInternal() override;

    virtual void onDownloaderStarted(Downloader* downloader);
    virtual void onDownloaderFinished(Downloader* downloader);
    virtual void onFirstTimeout();
    virtual void onSecondTimeout();

    void hookAnActiveDownloader(Downloader* downloader, bool unhook = false);
    void hookDownloaderOnInit(Downloader* downloader ,bool unhook = false);

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
};

#endif // DEFAULTDOWNLOADSTRATEGY_H
