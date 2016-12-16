/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include <QtNetwork>

#include "chunkeddownloader.h"

#include "timeoutexception.h"
#include "contentsummary.h"
#include "downloader.h"
#include "logger.h"

ChunkedDownloader::ChunkedDownloader(const ContentSummary& t_contentSummary, CancellationToken t_cancellationToken)
    : m_contentSummary(t_contentSummary)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_lastValidChunkIndex(0)
    , m_cancellationToken(t_cancellationToken)
    , m_currentReply(nullptr)
    , m_currentRequest(nullptr)
{
    connect(m_networkManager, &QNetworkAccessManager::networkAccessibleChanged, this, &ChunkedDownloader::onNetworkStatusChanged);
}

void ChunkedDownloader::waitForReply() const
{
    QEventLoop waitForReply;

    QTimer timeoutTimer;

    connect(m_currentReply, &QNetworkReply::readyRead, &waitForReply, &QEventLoop::quit);
    connect(&m_cancellationToken, &CancellationToken::cancelled, &waitForReply, &QEventLoop::quit);
    connect(&timeoutTimer, &QTimer::timeout, &waitForReply, &QEventLoop::quit);

    timeoutTimer.setInterval(m_requestTimeoutMsec);
    timeoutTimer.setSingleShot(true);
    timeoutTimer.start();

    waitForReply.exec();
    m_cancellationToken.throwIfCancelled();

    if (!timeoutTimer.isActive())
    {
        throw TimeoutException();
    }
}

void ChunkedDownloader::downloadFile(const QString & t_urlPath, const QString & t_filePath, int t_requestTimeoutMsec)
{
    QUrl url(t_urlPath);

    QNetworkConfigurationManager netManager;

    connect(&netManager, &QNetworkConfigurationManager::onlineStateChanged, this, &ChunkedDownloader::watchNetworkState);

    // Formulate the request
    m_currentRequest = new QNetworkRequest(url);

    // Request
    m_currentReply = m_networkManager->get(*m_currentRequest);
    connect(m_currentReply, &QNetworkReply::downloadProgress, this, &ChunkedDownloader::downloadProgressChanged);
    connect(m_currentReply, &QNetworkReply::downloadProgress, this, &ChunkedDownloader::watchDownloadProgress);

    while (!shouldStop())
    {
        try
        {
            waitForReply();

            waitForDownload();

            m_cancellationToken.throwIfCancelled();

            if (!validateReceivedData())
            {
                restartDownload();
            }
            else
            {
                break;
            }
        }
        catch (TimeoutException&)
        {

        }
    }

    writeDownloadedFile(t_filePath);
}

void ChunkedDownloader::watchNetworkState(bool isOnline)
{
    if (!isOnline)
    {
        logInfo("Connection lost - aborting request and retrying.");
        m_currentReply->abort();
    }
}

void ChunkedDownloader::waitForDownload()
{
    QEventLoop downloadLoop;

    connect(m_currentReply, &QNetworkReply::finished, &downloadLoop, &QEventLoop::quit);
    connect(&m_cancellationToken, &CancellationToken::cancelled, &downloadLoop, &QEventLoop::quit);

    downloadLoop.exec();

    m_cancellationToken.throwIfCancelled();
}

bool ChunkedDownloader::shouldStop()
{
    return false;
}

QVector<QByteArray> ChunkedDownloader::processChunks()
{
    QByteArray data = m_currentReply->readAll();
    QVector<QByteArray> chunks;

    for (int i = 0; i < data.size(); i += getChunkSize())
    {
        // Get the chunk
        QByteArray chunk = data.mid(i, getChunkSize());

        chunks.push_back(chunk);
    }

    return chunks;
}

bool ChunkedDownloader::validateReceivedData()
{
    QVector<QByteArray> chunks = processChunks();

    m_chunks += chunks;

    m_lastValidChunkIndex = m_chunks.size() - 1;

    for (int i = 0; i < m_chunks.size(); i++)
    {
        // TODO Validate the chunks
    }

    return m_chunks.size() == m_contentSummary.getChunksCount();
}

const int& ChunkedDownloader::getChunkSize() const
{
    return m_contentSummary.getChunkSize();
}

void ChunkedDownloader::watchDownloadProgress(const TByteCount& t_bytesDownloaded, const TByteCount& t_totalBytes)
{
    /*m_lastRecordedDownloadCounts.push(t_bytesDownloaded);

    if (m_lastRecordedDownloadCounts.size() > MAX_DOWNLOAD_QUEUE_SIZE)
        m_lastRecordedDownloadCounts.pop();

    if (isConnectionStalled())
        m_currentReply->abort();*/

    //logInfo("ChunkedDownloader watch download progress");

    /*if (m_networkManager->networkAccessible() == QNetworkAccessManager::NotAccessible)
        m_currentReply->abort();*/
}

bool ChunkedDownloader::isConnectionStalled()
{
    /*if (m_lastRecordedDownloadCounts.size() < MAX_DOWNLOAD_QUEUE_SIZE)
        return false;

    bool check = true;
    for (int i = 1; i < m_lastRecordedDownloadCounts.size(); i++)
    {
        if (m_lastRecordedDownloadCounts.at(i - 1) != m_lastRecordedDownloadCounts.at(i))

    }*/

    return false;
}

void ChunkedDownloader::writeDownloadedFile(const QString& t_filePath) const
{
    logInfo("Writing downloaded data to file - %1", .arg(t_filePath));

    QFile file(t_filePath);

    if (!file.open(QFile::WriteOnly))
    {
        throw std::runtime_error("Couldn't open file for download.");
    }

    QByteArray data;
    for (QByteArray chunk : m_chunks)
    {
        data.append(chunk);
    }

    file.write(data);

    file.close();
}

void ChunkedDownloader::restartDownload()
{
    m_currentReply->abort();
    disconnect(m_currentReply, &QNetworkReply::downloadProgress, this, &ChunkedDownloader::downloadProgressChanged);
    disconnect(m_currentReply, &QNetworkReply::downloadProgress, this, &ChunkedDownloader::watchDownloadProgress);
    delete m_currentReply;

    // Reformulate the request
    QByteArray header = "bytes=" + QByteArray::number((m_lastValidChunkIndex + 1) * getChunkSize()) + "-";

    m_currentRequest->setRawHeader("Range", header);

    m_currentReply = m_networkManager->get(*m_currentRequest);
    connect(m_currentReply, &QNetworkReply::downloadProgress, this, &ChunkedDownloader::downloadProgressChanged);
    connect(m_currentReply, &QNetworkReply::downloadProgress, this, &ChunkedDownloader::watchDownloadProgress);
}

void ChunkedDownloader::onNetworkStatusChanged(QNetworkAccessManager::NetworkAccessibility t_accessible)
{
    logWarning("Network status changed.");
    if (t_accessible == QNetworkAccessManager::NotAccessible)
    {
        m_currentReply->abort();
    }
}

ChunkedDownloader::~ChunkedDownloader()
{
    if (m_networkManager)
    {
        delete m_networkManager;
    }

    if (m_currentRequest)
    {
        delete m_currentRequest;
    }
}
