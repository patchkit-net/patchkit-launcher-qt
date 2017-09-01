/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "downloaderoperator.h"

#include "defaultdownloadstrategy.h"

#include "config.h"

DownloaderOperator::DownloaderOperator(Downloader::TDataSource t_dataSource, const IUrlProvider& t_urlProvider, CancellationToken t_cancellationToken, QObject* /*parent*/)
    : m_cancellationToken(t_cancellationToken)
{
    for (int i = 0; i < t_urlProvider.getVariantCount(); i++)
    {
        m_downloaders.push_back(new Downloader(t_urlProvider.getVariant(i), t_dataSource, t_cancellationToken));
    }
}

DownloaderOperator::~DownloaderOperator()
{
    for (Downloader* downloader : m_downloaders)
    {
        delete downloader;
    }
}

QByteArray DownloaderOperator::download(BaseDownloadStrategy& t_downloadStrategy)
{
    connect(&m_cancellationToken, &CancellationToken::cancelled, &t_downloadStrategy, &BaseDownloadStrategy::stop);
    connect(&t_downloadStrategy, &BaseDownloadStrategy::downloadProgress, this, &DownloaderOperator::downloadProgress);

    t_downloadStrategy.start();

    m_cancellationToken.throwIfCancelled();

    return t_downloadStrategy.data();
}

std::vector<Downloader*> DownloaderOperator::getActiveDownloaders() const
{
    return getDownloaders([](Downloader* downloader)
    {
        return downloader->isRunning();
    });
}

std::vector<Downloader*> DownloaderOperator::getStartingDownloaders() const
{
    return getDownloaders([](Downloader* downloader)
    {
        return downloader->wasStarted() && !downloader->isRunning();
    });
}

std::vector<Downloader*> DownloaderOperator::getInactiveDownloaders() const
{
    return getDownloaders([](Downloader* downloader)
    {
        return !downloader->wasStarted();
    });
}

std::vector<Downloader*> DownloaderOperator::getDownloaders(bool (*t_predicate)(Downloader*)) const
{
    std::vector<Downloader*> downloaders;

    for (Downloader* d : m_downloaders)
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
    for (Downloader* downloader : m_downloaders)
    {
        downloader->stop();
    }
}
