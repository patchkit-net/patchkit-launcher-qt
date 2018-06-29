#include "idownloaderpool.h"

std::vector<Downloader*> IDownloaderPool::getDownloaders(IDownloaderPool::TDownloaderPredicate t_predicate) const
{
    std::vector<Downloader*> pool;
    for (auto downloader : getDownloaders())
    {
        if (t_predicate(downloader))
        {
            pool.push_back(downloader);
        }
    }
    return pool;
}

void IDownloaderPool::append(const std::vector<Downloader*>& t_downloaders)
{
    for (auto downloader : t_downloaders)
    {
        add(downloader);
    }
}

std::vector<Downloader*> IDownloaderPool::getFinishedDownloaders() const
{
    return getDownloaders([](const Downloader* downloader)
    {
        return downloader->isFinished();
    });
}

std::vector<Downloader*> IDownloaderPool::getActiveDownloaders() const
{
    return getDownloaders([](const Downloader* downloader)
    {
        return downloader->isRunning() || downloader->isFinished();
    });
}

std::vector<Downloader*> IDownloaderPool::getStartingDownloaders() const
{
    return getDownloaders([](const Downloader* downloader)
    {
        return downloader->wasStarted() && !downloader->isRunning();
    });
}

std::vector<Downloader*> IDownloaderPool::getInactiveDownloaders() const
{
    return getDownloaders([](const Downloader* downloader)
    {
        return !downloader->wasStarted();
    });
}

size_t IDownloaderPool::poolSize() const
{
    return getDownloaders().size();
}
