#ifndef BASEDOWNLOADSTRATEGY_H
#define BASEDOWNLOADSTRATEGY_H

#include <QObject>

#include "idownloadstrategy.h"
#include "downloader.h"

class DownloaderOperator;

class BaseDownloadStrategy : public QObject, public IDownloadStrategy
{
    Q_OBJECT
public:
    void start(DownloaderOperator* t_operator);
    void end();
    QByteArray data();

public slots:
    void proceed();
    void stop();

signals:
    void done();
    void error(DownloadError t_error);
    void downloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes);

protected:
    virtual void init() = 0;
    virtual void finish() = 0;
    virtual void proceedInternal() = 0;
    virtual void stopInternal() = 0;

    QByteArray m_data;
    DownloaderOperator* m_operator;
};

#endif // BASEDOWNLOADSTRATEGY_H