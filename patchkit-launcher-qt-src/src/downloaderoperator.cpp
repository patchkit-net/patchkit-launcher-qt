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

void DownloaderOperator::add(Downloader* t_downloader)
{
    m_pool.add(t_downloader);
}

void DownloaderOperator::remove(Downloader* t_downloader)
{
    m_pool.remove(t_downloader);
}

Downloader* DownloaderOperator::waitForAnyToStart(CancellationToken t_cancellationToken, int t_timeoutMsec)
{
    if (getActiveDownloaders().size() > 0)
    {
        qInfo("Waiting for any downloader to start, but a downloader is already active.");
        return getActiveDownloaders().at(0);
    }

    if (getStartingDownloaders().size() == 0)
    {
        qWarning("Waiting for any downloader to start, but no downloaders were activated.");
        return nullptr;
    }

    auto startingDownloaders = getStartingDownloaders();

    QEventLoop loop;
    QTimer timeoutTimer;

    for (auto downloader : startingDownloaders)
    {
        connect(downloader, &Downloader::downloadStarted, &loop, &QEventLoop::quit);
    }

    connect(&t_cancellationToken, &CancellationToken::cancelled, &loop, &QEventLoop::quit);

    if (t_timeoutMsec != -1)
    {
        connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);

        timeoutTimer.setSingleShot(true);
        timeoutTimer.start(t_timeoutMsec);
    }

    loop.exec();

    t_cancellationToken.throwIfCancelled();

    auto activeDownloaders = getActiveDownloaders();

    if (activeDownloaders.size() == 0)
    {
        qWarning("Waited for any downloader to start, but no downloaders started.");
        return nullptr;
    }
    else
    {
        return activeDownloaders.at(0);
    }
}

Downloader* DownloaderOperator::waitForAnyToFinish(CancellationToken t_cancellationToken, int t_timeoutMsec)
{
    if (getFinishedDownloaders().size() > 0)
    {
        qInfo("Waiting for any downloader to finish, but a downloader already finished.");
        return getFinishedDownloaders().at(0);
    }

    if (getActiveDownloaders().size() == 0)
    {
        qInfo("Waiting for any downloader to finish, but none are active. Waiting for any to start.");

        auto startedDownloader = waitForAnyToStart(t_cancellationToken, t_timeoutMsec);

        if (startedDownloader)
        {
            qInfo("Successful start.");
        }
        else
        {
            qWarning("Failed to start within given timeout. Returning null.");
            return nullptr;
        }
    }

    auto activeDownloaders = getActiveDownloaders();

    QEventLoop loop;

    for (auto downloader : activeDownloaders)
    {
        connect(downloader, &Downloader::downloadFinished, &loop, &QEventLoop::quit);
    }

    connect(&t_cancellationToken, &CancellationToken::cancelled, &loop, &QEventLoop::quit);

    loop.exec();

    t_cancellationToken.throwIfCancelled();

    auto finishedDownloaders = getFinishedDownloaders();

    if (finishedDownloaders.size() == 0)
    {
        qWarning("Waited for any downloader to finish, but no downloaders have finished.");
        return nullptr;
    }
    else
    {
        return finishedDownloaders.at(0);
    }
}

void DownloaderOperator::startAll()
{
    for (auto downloader : getInactiveDownloaders())
    {
        downloader->start();
    }
}

void DownloaderOperator::stopAllExcept(Downloader* t_downloader)
{
    for (auto downloader : getDownloaders())
    {
        if (downloader != t_downloader)
        {
            downloader->stop();
        }
    }
}

void DownloaderOperator::stopAll()
{
    for (auto downloader : getDownloaders())
    {
        downloader->stop();
    }
}

void DownloaderOperator::setRange(int t_bytesStart, int t_bytesEnd)
{
    for (auto downloader : getDownloaders())
    {
        downloader->setRange(t_bytesStart, t_bytesEnd);
    }
}

QByteArray DownloaderOperator::download(BaseDownloadStrategy& t_downloadStrategy, CancellationToken t_cancellationToken)
{
    connect(&t_downloadStrategy, &BaseDownloadStrategy::downloadProgress, this, &DownloaderOperator::downloadProgress);

    t_downloadStrategy.start(t_cancellationToken);

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

DownloaderPool::DownloaderPool(const std::vector<Downloader*>& t_downloaders)
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

void DownloaderPool::add(Downloader* t_downloader)
{
    if (m_isIndependent)
    {
        qWarning("Adding a downloader to an independent pool, changes it's ownership.");
    }

    m_pool.push_back(t_downloader);
}

void DownloaderPool::remove(Downloader* t_downloader)
{
    if (m_isIndependent)
    {
        qWarning("Removing a downloader from an independent pool, changes it's ownership.");
    }

    m_pool.erase(std::remove(m_pool.begin(), m_pool.end(), t_downloader), m_pool.end());
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
    return getDownloaders([](Downloader* downloader)
    {
        return downloader->isFinished();
    });
}

std::vector<Downloader*> IDownloaderPool::getActiveDownloaders() const
{
    return getDownloaders([](Downloader* downloader)
    {
        return downloader->isRunning() || downloader->isFinished();
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

int IDownloaderPool::poolSize() const
{
    return getDownloaders().size();
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
