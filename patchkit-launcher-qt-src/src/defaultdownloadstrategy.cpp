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
    bool shouldStop = false;
    uint iterator = 1;
    Downloader* startedDownloader = nullptr;
    Downloader* finishedDownloader = nullptr;

    if (m_operator.poolSize() == 0)
    {
        throw std::runtime_error("Downloader pool is empty.");
    }

    auto startingPool = m_operator.getInactiveDownloaders();

    while (!shouldStop && !t_cancellationToken.isCancelled())
    {
        auto mainDownloader = startingPool.at(0);

        DownloaderOperator runningPool({mainDownloader});

        qDebug() << "Starting the first downloader.";
        runningPool.startAll();

        qDebug("Waiting for the first downloader to start.");
        startedDownloader = runningPool.waitForAnyToStart(t_cancellationToken, m_minTimeout);

        if (startedDownloader)
        {
            qDebug() << "The first downloader has started downloading, waiting for it to finish.";
            finishedDownloader = runningPool.waitForAnyToFinish(t_cancellationToken);
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
                finishedDownloader = runningPool.waitForAnyToFinish(t_cancellationToken);

                if (finishedDownloader)
                {
                    qInfo("Finished downloading.");
                    m_data = finishedDownloader->readData();
                    return;
                }
                else
                {
                    qWarning("Something went wrong.");
                    m_state.awaitResponseTo(DownloadError::ConnectionIssues, t_cancellationToken);
                }
            }
            else
            {
                qWarning("No downloader has started. Querying user for action.");
                m_state.awaitResponseTo(DownloadError::ConnectionIssues, t_cancellationToken);
            }
        }

        if (finishedDownloader)
        {
            qInfo("Finished downloading.");
            m_data = finishedDownloader->readData();
            return;
        }
        else
        {
            qWarning("Something went wrong.");
            m_state.awaitResponseTo(DownloadError::ConnectionIssues, t_cancellationToken);
        }
    }
}

const int DefaultDownloadStrategy::maxStartingDownloadersCount = 3;
