#ifndef BASEDOWNLOADSTRATEGY_H
#define BASEDOWNLOADSTRATEGY_H

#include <QObject>

#include "idownloadstrategy.h"
#include "downloader.h"

class BaseDownloadStrategy : public QObject, public IDownloadStrategy
{
    Q_OBJECT
public:
    virtual void init(const DownloaderOperator* t_operator) override;
    virtual void finish() override;

    virtual QByteArray data() override;

private slots:
    void onDownloaderStarted();
    void onDownloaderFinished();
    void onFirstTimeout();
    void onSecondTimeout();

signals:
    void done();
    void error();
    void downloadProgress(long long t_bytesDownloaded, long long t_totalBytes);

private:
    QByteArray m_data;
    const DownloaderOperator* m_operator;
};

#endif // BASEDOWNLOADSTRATEGY_H
