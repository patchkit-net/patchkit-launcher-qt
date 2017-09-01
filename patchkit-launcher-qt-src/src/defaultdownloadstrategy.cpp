#include "defaultdownloadstrategy.h"

#include "logger.h"

#include "downloaderoperator.h"

DefaultDownloadStrategy::DefaultDownloadStrategy(
        DownloaderOperator& t_operator,
        LauncherState& t_state,
        int t_minTimeout, int t_maxTimeout)
    : BaseDownloadStrategy(t_operator, t_state)
    , m_timeoutCounter(0)
    , m_iterator(0)
    , m_minTimeout(t_minTimeout)
    , m_maxTimeout(t_maxTimeout)
{
}

void DefaultDownloadStrategy::execute()
{
    QEventLoop loop;
    QTimer timer;
    connect(this, &BaseDownloadStrategy::stop, &loop, &QEventLoop::quit);

    while (true)
    {
        auto inactiveDownloaders = m_operator.getInactiveDownloaders();

        Downloader* firstDownloader = inactiveDownloaders.at(0);

        connect(firstDownloader, &Downloader::downloadError, &loop, &QEventLoop::quit);
        connect(firstDownloader, &Downloader::downloadFinished, &loop, &QEventLoop::quit);
        connect(firstDownloader, &Downloader::downloadStarted, &loop, &QEventLoop::quit);

        timer.setSingleShot(true);
        timer.start(m_minTimeout);

        connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

        firstDownloader->start();

        loop.exec();

        if (firstDownloader->isFinished())
        {
            m_data = firstDownloader->readData();
            return;
        }
        else if (firstDownloader->wasStarted())
        {

        }
        else if (firstDownloader->isRunning())
        {
            // Still processing
        }
        else // encountered an error
        {
            m_operator.stopAll();
            emit error(DownloadError::ConnectionIssues);
        }
    }
}

const int DefaultDownloadStrategy::maxStartingDownloadersCount = 3;
