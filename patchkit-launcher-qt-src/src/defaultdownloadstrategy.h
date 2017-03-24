#ifndef DEFAULTDOWNLOADSTRATEGY_H
#define DEFAULTDOWNLOADSTRATEGY_H

#include "basedownloadstrategy.h"

class DefaultDownloadStrategy : public BaseDownloadStrategy
{
    Q_OBJECT
public:
    DefaultDownloadStrategy(int t_minTimeout, int t_maxTimeout);

protected:
    virtual void init(const DownloaderOperator* t_operator) override;
    virtual void finish() override;

    virtual void proceedInternal() override;
    virtual void stopInternal() override;

protected slots:
    virtual void onDownloaderStarted();
    virtual void onDownloaderFinished();
    virtual void onFirstTimeout();
    virtual void onSecondTimeout();

private:
    QTimer m_timer;
    int m_timeoutCounter;

    void onTimeout();

    int m_iterator;
    int m_minTimeout;
    int m_maxTimeout;
};

#endif // DEFAULTDOWNLOADSTRATEGY_H
