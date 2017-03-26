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

QByteArray DownloaderOperator::download(BaseDownloadStrategy* t_downloadStrategy)
{
    if (!t_downloadStrategy)
    {
        DefaultDownloadStrategy baseDownloadStrategy(Config::minConnectionTimeoutMsec, Config::maxConnectionTimeoutMsec);
        return download(&baseDownloadStrategy);
    }

    t_downloadStrategy->start(this);

    connect(t_downloadStrategy, &BaseDownloadStrategy::downloadProgress, this, &DownloaderOperator::downloadProgress);

    QEventLoop loop;

    connect(t_downloadStrategy, &BaseDownloadStrategy::done, &loop, &QEventLoop::quit);
    connect(&m_cancellationToken, &CancellationToken::cancelled, &loop, &QEventLoop::quit);

    loop.exec();

    m_cancellationToken.throwIfCancelled();

    t_downloadStrategy->end();

    return t_downloadStrategy->data();
}

std::vector<Downloader*> DownloaderOperator::getActiveDownloaders() const
{
    return getDownloaders([](Downloader* downloader)
    {
        return downloader->isRunning();
    });
}

std::vector<Downloader*> DownloaderOperator::getStaleDownloaders() const
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
