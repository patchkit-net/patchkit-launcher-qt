/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <vector>
#include <deque>

#include <QVector>

#include "downloader.h"
#include "basedownloadstrategy.h"

#include "urlprovider.h"

class IDownloaderPool
{
public:
    typedef bool (*TDownloaderPredicate)(Downloader*);

    virtual std::vector<Downloader*> getDownloaders(TDownloaderPredicate t_predicate = nullptr) const = 0;

    std::vector<Downloader*> getFinishedDownloaders() const;
    std::vector<Downloader*> getActiveDownloaders() const;
    std::vector<Downloader*> getStartingDownloaders() const;
    std::vector<Downloader*> getInactiveDownloaders() const;
};

/**
 * @brief The DownloaderPool class
 */
class DownloaderPool : public IDownloaderPool
{
public:
    DownloaderPool(const DownloaderPool& t_downloaderPool);
    DownloaderPool(Downloader::TDataSource t_dataSource, const IUrlProvider& t_urlProvider, CancellationToken t_cancellationToken);
    DownloaderPool(std::initializer_list<Downloader*> t_downloaders);

    ~DownloaderPool();

    virtual std::vector<Downloader*> getDownloaders(TDownloaderPredicate t_predicate = nullptr) const override;

private:
    bool m_isIndependent;
    std::vector<Downloader*> m_pool;
};

/**
 * @brief The DownloaderOperator class
 */
class DownloaderOperator : public QObject, public IDownloaderPool
{
    Q_OBJECT
public:
    DownloaderOperator(Downloader::TDataSource t_dataSource, const IUrlProvider& t_urlProvider, CancellationToken t_cancellationToken, QObject* parent = nullptr);
    DownloaderOperator(std::initializer_list<Downloader*> t_downloaders, QObject* parent = nullptr);
    DownloaderOperator(const DownloaderPool& t_downloaderPool, QObject* parent = nullptr);

    virtual std::vector<Downloader*> getDownloaders(TDownloaderPredicate t_predicate = nullptr) const override;

    Downloader* waitForAnyToStart(CancellationToken t_cancellationToken, int t_timeoutMsec = -1);
    Downloader* waitForAnyToFinish(CancellationToken t_cancellationToken, int t_timeoutMsec = -1);

    void stopAllExcept(Downloader* t_downloader);
    void stopAll();

    QByteArray download(BaseDownloadStrategy& t_downloadStrategy, CancellationToken t_cancellationToken);

signals:
    void downloadProgress(long long t_bytesDownloaded, long long t_totalBytes);

private:
    DownloaderPool m_pool;

};
