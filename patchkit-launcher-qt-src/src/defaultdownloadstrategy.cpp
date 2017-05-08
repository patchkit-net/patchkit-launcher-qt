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

const int DefaultDownloadStrategy::maxStartingDownloadersCount = 3;

void DefaultDownloadStrategy::init()
{
    logInfo("Download strategy started.");

    auto inactiveDownloaders = m_operator->getInactiveDownloaders();
    auto allDownloaders = m_operator->getDownloaders();

    if (inactiveDownloaders.size() == 0)
    {
        logCritical("No inactive downloaders were available.");

        logDebug("Starting count: %1", .arg(m_startingDownloaders.size()));
        logDebug("Active count: %1", .arg(m_activeDownloaders.size()));

        logDebug("Downloaders debug info dump: ");
        for (Downloader* d : allDownloaders)
        {
            logDebug(d->debugInfo());
        }

        emit error(DownloadError::ConnectionIssues);
        return;
    }

    if (allDownloaders.size() != inactiveDownloaders.size())
    {
        logWarning("Some downloaders are still processing, errors may occur.");
    }

    inactiveDownloaders.at(m_iterator)->start();

    prepareDownloader(inactiveDownloaders.at(m_iterator));

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

void DefaultDownloadStrategy::onDownloaderStarted(Downloader* t_downloader)
{
    int statusCode = t_downloader->getStatusCode();

    if (!Downloader::doesStatusCodeIndicateSuccess(statusCode))
    {
        logInfo("Downloader failed to start, status code was: %1", .arg(statusCode));

        discardDownloader(t_downloader);
        t_downloader->stop();

        return;
    }

    if (!t_downloader->isRunning())
    {
        logWarning("The downloader is supposed to have started, but it's status shows otherwise.");
    }

    acceptActiveDownloader(t_downloader);

    logInfo("A downloader has started downloading.");
    for (Downloader* d : m_startingDownloaders)
    {
        discardDownloader(d);
        d->stop();
    }
}

void DefaultDownloadStrategy::onDownloaderFinished(Downloader* t_downloader)
{
    logInfo("A downloader finished downloading.");

    int statusCode = t_downloader->getStatusCode();

    if (!Downloader::doesStatusCodeIndicateSuccess(statusCode))
    {
        logWarning("A downloader has finished with a status code: %1", .arg(statusCode));

        if (m_operator->getStartingDownloaders().size() == 0 && m_operator->getActiveDownloaders().size() == 0)
        {
            logWarning("No pending or active downloaders, disabling the timer and emitting connection issues error.");

            m_timer.stop();
            emit error(DownloadError::ConnectionIssues);
            reset();
            return;
        }
    }

    logInfo("Download successful. Reseting and emitting done signal.");

    m_timer.stop();
    m_data = t_downloader->readData();

    reset();
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
    if (m_operator->getActiveDownloaders().size() == 0)
    {
        logWarning("No active downloaders after %1 ms, starting additional downloaders.", .arg(m_minTimeout));

        auto downloaders = m_operator->getInactiveDownloaders();

        for (int i = 0; i < (maxStartingDownloadersCount - 1) && i < downloaders.size(); i++)
        {
            downloaders.at(i)->start();
            prepareDownloader(downloaders.at(i));
        }
    }

    m_timer.setSingleShot(true);
    m_timer.start(m_maxTimeout);
}

void DefaultDownloadStrategy::onSecondTimeout()
{
    if (m_operator->getActiveDownloaders().size() == 0)
    {
        logWarning("No active downloaders after %1 ms, will emit connection issues error.", .arg(m_maxTimeout + m_minTimeout));

        reset();
        emit error(DownloadError::ConnectionIssues);
    }
}

void DefaultDownloadStrategy::prepareDownloader(Downloader* t_downloader)
{
    if (m_startingDownloaders.contains(t_downloader))
    {
        logWarning("Downloader already prepared.");
        return;
    }

    m_startingDownloaders.push_back(t_downloader);

    connect(t_downloader, &Downloader::downloadStarted, this, &DefaultDownloadStrategy::onDownloaderStartedInternal);
}

void DefaultDownloadStrategy::discardDownloader(Downloader* t_downloader)
{
    if (!m_startingDownloaders.contains(t_downloader))
    {
        logWarning("Couldn't find the downloader in the starting downloaders list.");
        return;
    }

    m_startingDownloaders.removeOne(t_downloader);

    disconnect(t_downloader, &Downloader::downloadStarted, this, &DefaultDownloadStrategy::onDownloaderStartedInternal);

    t_downloader->stop();
}

void DefaultDownloadStrategy::acceptActiveDownloader(Downloader* t_downloader)
{
    if (m_activeDownloaders.contains(t_downloader))
    {
        logWarning("Downloader has already been accepted.");
        return;
    }

    m_startingDownloaders.removeOne(t_downloader);

    disconnect(t_downloader, &Downloader::downloadStarted, this, &DefaultDownloadStrategy::onDownloaderStartedInternal);

    m_activeDownloaders.push_back(t_downloader);

    connect(t_downloader, &Downloader::downloadFinished, this, &DefaultDownloadStrategy::onDownloaderFinishedInternal);
    connect(t_downloader, &Downloader::downloadProgressChanged, this, &BaseDownloadStrategy::downloadProgressRelay);
}

void DefaultDownloadStrategy::discardActiveDownloader(Downloader* t_downloader)
{
    if (!m_activeDownloaders.contains(t_downloader))
    {
        logWarning("Couldn't find the downloader in the active downloaders list.");
        return;
    }

    m_activeDownloaders.removeOne(t_downloader);

    disconnect(t_downloader, &Downloader::downloadFinished, this, &DefaultDownloadStrategy::onDownloaderFinishedInternal);
    disconnect(t_downloader, &Downloader::downloadProgressChanged, this, &BaseDownloadStrategy::downloadProgress);

    t_downloader->stop();
}

void DefaultDownloadStrategy::reset()
{
    for (Downloader* downloader : m_activeDownloaders)
    {
        discardActiveDownloader(downloader);
    }

    m_activeDownloaders.clear();

    for (Downloader* downloader : m_startingDownloaders)
    {
        discardDownloader(downloader);
    }

    m_startingDownloaders.clear();
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
