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

void ChunkedDownloadStrategy::finish()
{
    DefaultDownloadStrategy::finish();
}

void ChunkedDownloadStrategy::onDownloaderFinished(Downloader* downloader)
{
    logInfo("Chunked download strategy - a downloader finished downloading, processing downloaded data.");

    logDebug("Downloader name is: %1", .arg(downloader->debugName()));

    hookAnActiveDownloader(downloader ,false);

    QByteArray data = m_data + downloader->readData();

    QVector<QByteArray> chunks = m_parent.processChunks(data);
    QVector<bool> validChunkMap = m_parent.validateAllChunks(chunks);
    int validChunkCount = m_parent.m_contentSummary.getChunksCount();

    if (validChunkMap.size() == validChunkCount && !validChunkMap.contains(false))
    {
        logInfo("Downloaded data is valid and complete.");
        m_data = data;
        emit done();
    }
    else
    {
        int firstInvalidChunkIndex = validChunkMap.indexOf(false);

        if (firstInvalidChunkIndex == -1)
        {
            logInfo("Downloaded data is valid but incomplete.");
            setRanges(m_parent.getChunkSize() * validChunkMap.size());
            m_data.clear();

            for (QByteArray& chunk : chunks)
            {
                m_data += chunk;
            }
        }
        else if (firstInvalidChunkIndex != 0)
        {
            logInfo("Downloaded data contains invalid chunks or is incomplete.");
            setRanges(m_parent.getChunkSize() * firstInvalidChunkIndex);
            m_data.clear();

            for (int i = 0 ; i < firstInvalidChunkIndex; i++)
            {
                m_data += chunks.at(i);
            }
        }

        reset();
        init();
    }
}

void ChunkedDownloadStrategy::setRanges(int t_from)
{
    for (auto downloader : m_operator->getDownloaders())
    {
        downloader->setRange(t_from);
    }
}
