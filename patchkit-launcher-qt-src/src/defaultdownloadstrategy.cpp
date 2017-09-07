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

void DefaultDownloadStrategy::execute(CancellationToken t_cancellationToken)
{
    uint iterator = 1;

    if (m_operator.poolSize() == 0)
    {
        throw std::runtime_error("Downloader pool is empty.");
    }

    auto startingPool = m_operator.getInactiveDownloaders();

    while (!t_cancellationToken.isCancelled())
    {
        Downloader* startedDownloader = nullptr;
        Downloader* finishedDownloader = nullptr;

        auto mainDownloader = startingPool.at(0);

        DownloaderOperator runningPool({mainDownloader});

        qDebug() << "Starting the first downloader.";
        runningPool.startAll();

        qDebug("Waiting for the first downloader to start.");
        startedDownloader = runningPool.waitForAnyToStart(t_cancellationToken, m_minTimeout);

        if (startedDownloader)
        {
            qDebug() << "The first downloader has started downloading, waiting for it to finish.";
            watchProgressOf(startedDownloader);
            finishedDownloader = runningPool.waitForAnyToFinish(t_cancellationToken);
            stopWatchingProgressOf(startedDownloader);

            if (finishedDownloader)
            {
                if (processFinishedDownloader(finishedDownloader))
                {
                    return;
                }
                else
                {
                    runningPool.stopAll();
                }
            }
            else
            {
                qWarning("Connection issues.");
                onConnectionIssues(t_cancellationToken);
            }
        }
        else
        {
            qDebug() << "The first downloader didn't start, trying up to 2 other nodes.";
            for (uint i = iterator; i < startingPool.size() && i < (iterator + maxStartingDownloadersCount - 1); i++)
            {
                runningPool.add(startingPool.at(i));
            }

            iterator += 2;

            if (iterator >= startingPool.size())
            {
                iterator -= startingPool.size();
            }

            runningPool.startAll();
            startedDownloader = runningPool.waitForAnyToStart(t_cancellationToken, m_maxTimeout);

            if (startedDownloader)
            {
                qDebug() << "A downloader has started, stopping all others and waiting for this one to finish.";
                runningPool.stopAllExcept(startedDownloader);
                watchProgressOf(startedDownloader);
                finishedDownloader = runningPool.waitForAnyToFinish(t_cancellationToken);
                stopWatchingProgressOf(startedDownloader);

                if (finishedDownloader)
                {
                    qInfo("Finished downloading.");
                    if (processFinishedDownloader(finishedDownloader))
                    {
                        return;
                    }
                    else
                    {
                        runningPool.stopAll();
                    }
                }
                else
                {
                    qWarning("Connection issues.");
                    onConnectionIssues(t_cancellationToken);
                }
            }
            else
            {
                qWarning("No downloader has started. Querying user for action.");
                onConnectionIssues(t_cancellationToken);
            }
        }
    }
}

bool DefaultDownloadStrategy::processFinishedDownloader(Downloader* t_downloader)
{
    m_data = t_downloader->readData();
    return true;
}

void DefaultDownloadStrategy::onConnectionIssues(CancellationToken t_cancellationToken)
{
    m_state.awaitResponseTo(DownloadError::ConnectionIssues, t_cancellationToken);
    m_operator.stopAll();
}

const int DefaultDownloadStrategy::maxStartingDownloadersCount = 3;
