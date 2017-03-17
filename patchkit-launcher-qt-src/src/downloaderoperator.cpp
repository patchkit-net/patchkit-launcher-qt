#include "downloaderoperator.h"

DownloaderOperator::DownloaderOperator(Downloader::TDataSource t_dataSource, const IResourceUrlProvider& urlProvider, CancellationToken t_cancellationToken, QObject* /*parent*/)
    : m_cancellationToken(t_cancellationToken)
{
    for (int i = 0; i < urlProvider.getVariantCount(); i++)
    {
        m_downloaders.push_back(new Downloader(urlProvider.getVariant(i), t_dataSource, t_cancellationToken));
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
        BaseDownloadStrategy baseDownloadStrategy;
        return download(&baseDownloadStrategy);
    }

    t_downloadStrategy->init(this);

    connect(t_downloadStrategy, &BaseDownloadStrategy::downloadProgress, this, &DownloaderOperator::downloadProgress);

    QEventLoop loop;

    connect(t_downloadStrategy, &BaseDownloadStrategy::done, &loop, &QEventLoop::quit);
    connect(&m_cancellationToken, &CancellationToken::cancelled, &loop, &QEventLoop::quit);

    loop.exec();

    m_cancellationToken.throwIfCancelled();

    t_downloadStrategy->finish();

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
