#include "basedownloadstrategy.h"

#include "downloaderoperator.h"

#include <QMessageBox>

void BaseDownloadStrategy::init(const DownloaderOperator* t_operator)
{
    m_operator = t_operator;

    std::vector<Downloader*> downloaders = t_operator->getInactiveDownloaders();

    if (downloaders.size() == 0)
    {
        throw std::exception("No downloaders were available.");
    }

    downloaders.at(0)->start();

    connect(downloaders.at(0), &Downloader::downloadStarted, this, &BaseDownloadStrategy::onDownloaderStarted);
    QTimer::singleShot(10000, this, &BaseDownloadStrategy::onFirstTimeout);
    QTimer::singleShot(30000, this, &BaseDownloadStrategy::onSecondTimeout);
}

void BaseDownloadStrategy::finish()
{
}

QByteArray BaseDownloadStrategy::data()
{
    return m_data;
}

void BaseDownloadStrategy::onDownloaderStarted()
{
    // Recover the sender
    Downloader* downloader = static_cast<Downloader*> (sender());

    auto staleDownloaders = m_operator->getStaleDownloaders();
    for (Downloader* d : staleDownloaders)
    {
        disconnect(d, &Downloader::downloadStarted, this, &BaseDownloadStrategy::onDownloaderStarted);
        d->stop();
    }

    disconnect(downloader, &Downloader::downloadStarted, this, &BaseDownloadStrategy::onDownloaderStarted);

    connect(downloader, &Downloader::downloadFinished, this, &BaseDownloadStrategy::onDownloaderFinished);
    connect(downloader, &Downloader::downloadProgressChanged, this, &BaseDownloadStrategy::downloadProgress);
}

void BaseDownloadStrategy::onDownloaderFinished()
{
    Downloader* downloader = static_cast<Downloader*> (sender());

    m_data = downloader->readData();

    disconnect(downloader, &Downloader::downloadFinished, this, &BaseDownloadStrategy::onDownloaderFinished);
    disconnect(downloader, &Downloader::downloadProgressChanged, this, &BaseDownloadStrategy::downloadProgress);

    emit done();
}

void BaseDownloadStrategy::onFirstTimeout()
{
    if (m_operator->getActiveDownloaders().size() == 0)
    {
        auto downloaders = m_operator->getInactiveDownloaders();

        if (downloaders.size() >= 2)
        {
            downloaders.at(0)->start();
            connect(downloaders.at(1), &Downloader::downloadStarted, this, &BaseDownloadStrategy::onDownloaderStarted);

            downloaders.at(1)->start();
            connect(downloaders.at(0), &Downloader::downloadStarted, this, &BaseDownloadStrategy::onDownloaderStarted);
        }
    }
}

void BaseDownloadStrategy::onSecondTimeout()
{
    if (m_operator->getActiveDownloaders().size() == 0)
    {
        emit error();
    }
}
