#include "chunkeddownloadstrategy.h"

#include "chunkeddownloader.h"
#include "downloaderoperator.h"

#include "logger.h"
#include "contentsummary.h"
#include "downloader.h"

ChunkedDownloadStrategy::ChunkedDownloadStrategy(
        DownloaderOperator& t_operator,
        int t_minTimeout,
        int t_maxTimeout,
        const ChunkedDownloader& t_parent)
    : DefaultDownloadStrategy(t_operator, t_minTimeout, t_maxTimeout)
    , m_parent(t_parent)
{
}

bool ChunkedDownloadStrategy::processFinishedDownloader(Downloader* t_downloader)
{
    qInfo("Chunked download strategy - a downloader finished downloading, processing downloaded data.");

    qDebug() << "Downloader name is: " << t_downloader->debugName();

    QByteArray data = m_data + t_downloader->readData();

    QVector<QByteArray> chunks = m_parent.processChunks(data);
    QVector<bool> validChunkMap = m_parent.validateAllChunks(chunks);
    int validChunkCount = m_parent.m_contentSummary.getChunksCount();

    if (validChunkMap.size() == validChunkCount && !validChunkMap.contains(false))
    {
        qInfo("Downloaded data is valid and complete.");
        m_data = data;
        return true;
    }
    else
    {
        int firstInvalidChunkIndex = validChunkMap.indexOf(false);

        if (firstInvalidChunkIndex == -1)
        {
            qInfo("Downloaded data is valid but incomplete.");
            setRanges(m_parent.getChunkSize() * validChunkMap.size());
            m_data.clear();

            for (QByteArray& chunk : chunks)
            {
                m_data += chunk;
            }

            return false;
        }
        else if (firstInvalidChunkIndex != 0)
        {
            qInfo("Downloaded data contains invalid chunks or is incomplete.");
            setRanges(m_parent.getChunkSize() * firstInvalidChunkIndex);
            m_data.clear();

            for (int i = 0 ; i < firstInvalidChunkIndex; i++)
            {
                m_data += chunks.at(i);
            }

            return false;
        }
        else
        {
            qInfo("Downloaded data is completely invalid.");
            return false;
        }
    }
}

void ChunkedDownloadStrategy::downloadProgressRelay(const long long& t_bytesDownloaded, const long long& t_totalBytes)
{
    long long bytesProcessed = m_data.size();

    emit downloadProgress(t_bytesDownloaded + bytesProcessed, t_totalBytes + bytesProcessed);
}

void ChunkedDownloadStrategy::setRanges(int t_from)
{
    m_operator.setRange(t_from);
}
