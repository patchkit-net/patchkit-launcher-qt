/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "downloaderoperator.h"

#include "defaultdownloadstrategy.h"

#include "config.h"

DownloaderOperator::DownloaderOperator(Downloader::TDataSource t_dataSource, const IUrlProvider& t_urlProvider, CancellationToken t_cancellationToken, QObject* parent)
    : QObject(parent)
    , m_pool(t_dataSource, t_urlProvider, t_cancellationToken)
{
}

DownloaderOperator::DownloaderOperator(std::initializer_list<Downloader*> t_downloaders, QObject* parent)
    : QObject(parent)
    , m_pool(t_downloaders)
{

}

DownloaderOperator::DownloaderOperator(const DownloaderPool& t_downloaderPool, QObject* parent)
    : QObject(parent)
    , m_pool(t_downloaderPool)
{
}

std::vector<Downloader*> DownloaderOperator::getDownloaders(IDownloaderPool::TDownloaderPredicate t_predicate) const
{
    return m_pool.getDownloaders(t_predicate);
}

QByteArray DownloaderOperator::download(BaseDownloadStrategy& t_downloadStrategy, CancellationToken t_cancellationToken)
{
    connect(&t_cancellationToken, &CancellationToken::cancelled, &t_downloadStrategy, &BaseDownloadStrategy::stop);
    connect(&t_downloadStrategy, &BaseDownloadStrategy::downloadProgress, this, &DownloaderOperator::downloadProgress);

    t_downloadStrategy.start();

    t_cancellationToken.throwIfCancelled();

    return t_downloadStrategy.data();
}

DownloaderPool::DownloaderPool(const DownloaderPool& t_downloaderPool)
    : m_isIndependent(false)
    , m_pool(t_downloaderPool.m_pool)
{
}

DownloaderPool::DownloaderPool(Downloader::TDataSource t_dataSource, const IUrlProvider& t_urlProvider, CancellationToken t_cancellationToken)
    : m_isIndependent(true)
{
    for (int i = 0; i < t_urlProvider.getVariantCount(); i++)
    {
        m_pool.push_back(new Downloader(t_urlProvider.getVariant(i), t_dataSource, t_cancellationToken));
    }
}

DownloaderPool::DownloaderPool(std::initializer_list<Downloader*> t_downloaders)
    : m_isIndependent(false)
    , m_pool(t_downloaders)
{

}

DownloaderPool::~DownloaderPool()
{
    if (m_isIndependent)
    {
        for (Downloader* d : m_pool)
        {
            delete d;
        }
    }
}

std::vector<Downloader*> IDownloaderPool::getActiveDownloaders() const
{
    return getDownloaders([](Downloader* downloader)
    {
        return downloader->isRunning();
    });
}

std::vector<Downloader*> IDownloaderPool::getStartingDownloaders() const
{
    return getDownloaders([](Downloader* downloader)
    {
        return downloader->wasStarted() && !downloader->isRunning();
    });
}

std::vector<Downloader*> IDownloaderPool::getInactiveDownloaders() const
{
    return getDownloaders([](Downloader* downloader)
    {
        return !downloader->wasStarted();
    });
}

std::vector<Downloader*> DownloaderPool::getDownloaders(bool (*t_predicate)(Downloader*)) const
{
    std::vector<Downloader*> downloaders;

    for (Downloader* d : m_pool)
    {
        if (!t_predicate || t_predicate(d))
        {
            downloaders.push_back(d);
        }
    }

    return downloaders;
}

void DownloaderOperator::stopAll()
{
    for (Downloader* downloader : getDownloaders())
    {
        downloader->stop();
    }
}
