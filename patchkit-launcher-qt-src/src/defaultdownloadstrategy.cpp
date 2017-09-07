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

        runningPool.startAll();

        startedDownloader = runningPool.waitForAnyToStart(t_cancellationToken, m_minTimeout);

        if (startedDownloader)
        {
            finishedDownloader = runningPool.waitForAnyToFinish(t_cancellationToken);
        }
        else
        {
            for (uint i = iterator; i < startingPool.size() && i < iterator + 2; i++)
            {
                runningPool.add(startingPool.at(i));
            }

            iterator += 2;

            if (iterator >= startingPool.size())
            {
                iterator -= startingPool.size();
            }

            startedDownloader = runningPool.waitForAnyToStart(t_cancellationToken, m_maxTimeout);

            if (startedDownloader)
            {
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
                }
            }
            else
            {
                qWarning("No downloader has started. Querying user for action.");
                auto response = m_state.awaitResponseTo(DownloadError::ConnectionIssues, t_cancellationToken);

                if (response == LauncherState::Response::Stop)
                {

                }
                else if (response == LauncherState::Response::Proceed)
                {

                }
            }
        }

        if (finishedDownloader)
        {

        }
        else
        {

        }
    }
}

const int DefaultDownloadStrategy::maxStartingDownloadersCount = 3;
