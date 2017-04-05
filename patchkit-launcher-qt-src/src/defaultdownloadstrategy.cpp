#include "defaultdownloadstrategy.h"

#include "logger.h"

#include "downloaderoperator.h"

DefaultDownloadStrategy::DefaultDownloadStrategy(int t_minTimeout, int t_maxTimeout)
    : m_minTimeout(t_minTimeout)
    , m_maxTimeout(t_maxTimeout)
    , m_iterator(0)
    , m_timeoutCounter(0)
{
    connect(&m_timer, &QTimer::timeout, this, &DefaultDownloadStrategy::onTimeout);
}

void DefaultDownloadStrategy::init()
{
    logInfo("Download strategy started.");

    auto inactiveDownloaders = m_operator->getInactiveDownloaders();
    auto allDownloaders = m_operator->getDownloaders();

    if (inactiveDownloaders.size() == 0)
    {
        logCritical("No downloaders were available.");
        emit error(DownloadError::ConnectionIssues);
        return;
    }

    if (allDownloaders.size() != inactiveDownloaders.size())
    {
        logWarning("Some downloaders are still processing, errors may occur.");
    }

    inactiveDownloaders.at(m_iterator)->start();

    hookDownloaderOnInit(inactiveDownloaders.at(m_iterator));

    m_timer.setSingleShot(true);
    m_timer.start(m_minTimeout);
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

    init();
}

void DefaultDownloadStrategy::stopInternal()
{
    logInfo("Stop internal requested.");
    emit done();
}

void DefaultDownloadStrategy::onDownloaderStarted(Downloader* downloader)
{
    hookDownloaderOnInit(downloader, true);

    int statusCode = downloader->getStatusCode();

    if (!Downloader::doesStatusCodeIndicateSuccess(statusCode))
    {
        logInfo("Downloader failed to start, status code was: %1", .arg(statusCode));

        downloader->stop();

        return;
    }

    if (!downloader->isRunning())
    {
        logWarning("The downloader is supposed to have started, but it's status shows otherwise.");
    }

    logInfo("A downloader has started downloading.");
    auto staleDownloaders = m_operator->getStaleDownloaders();
    for (Downloader* d : staleDownloaders)
    {
        hookDownloaderOnInit(d, false);
        d->stop();
    }

    hookAnActiveDownloader(downloader);
}

void DefaultDownloadStrategy::onDownloaderFinished(Downloader* downloader)
{
    logInfo("A downloader finished downloading.");

    int statusCode = downloader->getStatusCode();

    hookAnActiveDownloader(downloader, true);

    if (!Downloader::doesStatusCodeIndicateSuccess(statusCode))
    {
        logWarning("A downloader has finished with a status code: %1", .arg(statusCode));

        if (m_operator->getStaleDownloaders().size() == 0 && m_operator->getActiveDownloaders().size() == 0)
        {
            logWarning("No pending or active downloaders, disabling the timer and emitting connection issues error.");

            m_timer.stop();
            emit error(DownloadError::ConnectionIssues);
            reset();
            return;
        }
    }

    m_timer.stop();
    m_data = downloader->readData();

    m_operator->stopAll();

    emit done();
}

void DefaultDownloadStrategy::onDownloaderStartedInternal()
{
    // Recover the sender
    Downloader* downloader = static_cast<Downloader*> (sender());

    onDownloaderStarted(downloader);
}

void DefaultDownloadStrategy::onDownloaderFinishedInternal()
{
    Downloader* downloader = static_cast<Downloader*> (sender());

    onDownloaderFinished(downloader);
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
            hookDownloaderOnInit(downloaders.at(i));
        }
    }

    m_timer.setSingleShot(true);
    m_timer.start(m_maxTimeout);
}

void DefaultDownloadStrategy::onSecondTimeout()
{
    logInfo("Second timeout.");
    if (m_operator->getActiveDownloaders().size() == 0)
    {
        for (Downloader* d : m_operator->getStaleDownloaders())
        {
            d->stop();
            hookDownloaderOnInit(d, true);
        }
        emit error(DownloadError::ConnectionIssues);
    }
}

void DefaultDownloadStrategy::hookAnActiveDownloader(Downloader* downloader, bool unhook)
{
    if (unhook)
    {
        disconnect(downloader, &Downloader::downloadFinished, this, &DefaultDownloadStrategy::onDownloaderFinishedInternal);
        disconnect(downloader, &Downloader::downloadProgressChanged, this, &DefaultDownloadStrategy::downloadProgress);
    }
    else
    {
        connect(downloader, &Downloader::downloadFinished, this, &DefaultDownloadStrategy::onDownloaderFinishedInternal);
        connect(downloader, &Downloader::downloadProgressChanged, this, &DefaultDownloadStrategy::downloadProgress);
    }
}

void DefaultDownloadStrategy::hookDownloaderOnInit(Downloader* downloader, bool unhook)
{
    if (unhook)
    {
        disconnect(downloader, &Downloader::downloadStarted, this, &DefaultDownloadStrategy::onDownloaderStartedInternal);
    }
    else
    {
        connect(downloader, &Downloader::downloadStarted, this, &DefaultDownloadStrategy::onDownloaderStartedInternal);
    }
}

void DefaultDownloadStrategy::reset()
{
    m_operator->stopAll();
}

void DefaultDownloadStrategy::printDebugInfo()
{
    QString base("Download startegy debug info -- START");

    for (Downloader* d : m_operator->getDownloaders())
    {
        base.append("\n");
        base.append(d->debugInfo());
    }

    logDebug(base);
    logDebug("Download startegy debug info -- END");
}
