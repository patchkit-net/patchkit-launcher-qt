#include "defaultdownloadstrategy.h"

#include "logger.h"

#include "downloaderoperator.h"

DefaultDownloadStrategy::DefaultDownloadStrategy(int t_minTimeout, int t_maxTimeout)
    : m_minTimeout(t_minTimeout)
    , m_maxTimeout(t_maxTimeout)
    , m_iterator(0)
    , m_timeoutCounter(0)
{
}

void DefaultDownloadStrategy::init(const DownloaderOperator* t_operator)
{
    logInfo("Download strategy started.");

    m_operator = t_operator;

    auto inactiveDownloaders = t_operator->getInactiveDownloaders();
    auto allDownloaders = t_operator->getDownloaders();

    if (inactiveDownloaders.size() == 0)
    {
        throw std::exception("No downloaders were available.");
    }

    if (allDownloaders.size() != inactiveDownloaders.size())
    {
        logWarning("Some downloaders have not been reset, issues may occur.");
    }

    inactiveDownloaders.at(m_iterator)->start();

    connect(inactiveDownloaders.at(m_iterator), &Downloader::downloadStarted, this, &DefaultDownloadStrategy::onDownloaderStarted);

    m_timer.setSingleShot(true);
    m_timer.setInterval(m_minTimeout);
    m_timer.start();
    connect(&m_timer, &QTimer::timeout, this, &DefaultDownloadStrategy::onTimeout);
}

void DefaultDownloadStrategy::onTimeout()
{
    if (m_timeoutCounter % 2 == 0)
    {
        onFirstTimeout();
    }
    else
    {
        onSecondTimeout();
    }
    m_timeoutCounter++;
}

void DefaultDownloadStrategy::finish()
{
    logInfo("Download strategy finished.");
}

void DefaultDownloadStrategy::proceedInternal()
{
    logInfo("Proceed internal was requested.");
    m_iterator += 1;

    if (m_iterator >= m_operator->getDownloaders().size())
    {
        m_iterator = 0;
    }

    init(m_operator);
}

void DefaultDownloadStrategy::stopInternal()
{
    emit done();
}

void DefaultDownloadStrategy::onDownloaderStarted()
{
    // Recover the sender
    Downloader* downloader = static_cast<Downloader*> (sender());

    int statusCode = downloader->getStatusCode();

    if (!Downloader::doesStatusCodeIndicateSuccess(statusCode))
    {
        logInfo("Downloader failed to start, status code was: %1", .arg(statusCode));

        disconnect(downloader, &Downloader::downloadStarted, this, &DefaultDownloadStrategy::onDownloaderStarted);
        downloader->stop();

        return;
    }

    logInfo("Downloader has started downloading.");
    auto staleDownloaders = m_operator->getStaleDownloaders();
    for (Downloader* d : staleDownloaders)
    {
        disconnect(d, &Downloader::downloadStarted, this, &DefaultDownloadStrategy::onDownloaderStarted);
        d->stop();
    }

    disconnect(downloader, &Downloader::downloadStarted, this, &DefaultDownloadStrategy::onDownloaderStarted);

    connect(downloader, &Downloader::downloadFinished, this, &DefaultDownloadStrategy::onDownloaderFinished);
    connect(downloader, &Downloader::downloadProgressChanged, this, &DefaultDownloadStrategy::downloadProgress);
}

void DefaultDownloadStrategy::onDownloaderFinished()
{
    Downloader* downloader = static_cast<Downloader*> (sender());

    int statusCode = downloader->getStatusCode();

    disconnect(downloader, &Downloader::downloadFinished, this, &DefaultDownloadStrategy::onDownloaderFinished);
    disconnect(downloader, &Downloader::downloadProgressChanged, this, &DefaultDownloadStrategy::downloadProgress);

    if (!Downloader::doesStatusCodeIndicateSuccess(statusCode))
    {
        logWarning("A downloader has finished with a status code: %1", .arg(statusCode));

        if (m_operator->getStaleDownloaders().size() == 0)
        {
            logDebug("No pending downloaders, disabling the timer and emitting connection issues error.");

            m_timer.stop();

            emit error(DownloadError::ConnectionIssues);
        }
    }

    logInfo("Downloader has finished downloading.");

    m_data = downloader->readData();

    emit done();
}

void DefaultDownloadStrategy::onFirstTimeout()
{
    logInfo("First timeout.");
    if (m_operator->getActiveDownloaders().size() == 0)
    {
        auto downloaders = m_operator->getInactiveDownloaders();

        for (int i = 0; i < 2 && i < downloaders.size(); i++)
        {
            downloaders.at(i)->start();
            connect(downloaders.at(i), &Downloader::downloadStarted, this, &DefaultDownloadStrategy::onDownloaderStarted);
        }
    }

    m_timer.setInterval(m_maxTimeout - m_minTimeout);
    m_timer.start();
}

void DefaultDownloadStrategy::onSecondTimeout()
{
    logInfo("Second timeout.");
    if (m_operator->getActiveDownloaders().size() == 0)
    {
        for (Downloader* d : m_operator->getStaleDownloaders())
        {
            disconnect(d,&Downloader::downloadStarted, this, &DefaultDownloadStrategy::onDownloaderStarted);
        }
        emit error(DownloadError::ConnectionIssues);
    }
}
