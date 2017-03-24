#include "chunkeddownloadstrategy.h"

#include "chunkeddownloader.h"

#include "logger.h"

#include "downloaderoperator.h"

#include "contentsummary.h"

#include "downloader.h"

ChunkedDownloadStrategy::ChunkedDownloadStrategy(int t_minTimeout, int t_maxTimeout, const ChunkedDownloader& t_parent)
    : DefaultDownloadStrategy(t_minTimeout, t_maxTimeout)
    , m_parent(t_parent)
{
}

void ChunkedDownloadStrategy::init(const DownloaderOperator* t_operator)
{
    DefaultDownloadStrategy::init(t_operator);
}

void ChunkedDownloadStrategy::finish()
{
    DefaultDownloadStrategy::finish();
}

void ChunkedDownloadStrategy::onDownloaderFinished()
{
    Downloader* downloader = static_cast<Downloader*> (sender());

    disconnect(downloader, &Downloader::downloadFinished, this, &ChunkedDownloadStrategy::onDownloaderFinished);
    disconnect(downloader, &Downloader::downloadProgressChanged, this, &DefaultDownloadStrategy::downloadProgress);

    QByteArray data = m_data + downloader->readData();

    QVector<QByteArray> chunks = m_parent.processChunks(data);
    QVector<bool> validChunkMap = m_parent.validateAllChunks(chunks);
    int validChunkCount = m_parent.m_contentSummary.getChunksCount();

    if (validChunkMap.size() == validChunkCount && !validChunkMap.contains(false))
    {
        m_data = data;
        emit done();
    }
    else
    {
        int firstInvalidChunkIndex = validChunkMap.indexOf(false);

        if (firstInvalidChunkIndex == -1)
        {
            setRanges(m_parent.getChunkSize() * validChunkMap.size());
        }
        else if (firstInvalidChunkIndex != 0)
        {
            setRanges(m_parent.getChunkSize() * firstInvalidChunkIndex);
        }

        for (auto downloader : m_operator->getDownloaders())
        {
            downloader->stop();
        }

        DefaultDownloadStrategy::init(m_operator);
    }
}

void ChunkedDownloadStrategy::setRanges(int t_from)
{
    for (auto downloader : m_operator->getDownloaders())
    {
        downloader->setRange(t_from);
    }
}
