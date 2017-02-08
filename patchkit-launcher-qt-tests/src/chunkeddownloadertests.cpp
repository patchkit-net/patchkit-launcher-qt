/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "catch.h"

#include <QtNetwork>
#include <QApplication>

#include "src/chunkeddownloader.h"
#include "src/timeoutexception.h"
#include "src/contentsummary.h"
#include "src/staledownloadexception.h"

#include "mockednam.h"

SCENARIO("Testing chunked downloader in multiple scenarios.", "[chunked_downloader]")
{
    std::shared_ptr<CancellationTokenSource> tokenSource(new CancellationTokenSource());
    CancellationToken token(tokenSource);

    GIVEN("A constant data set in chunks with a content summary.")
    {
        const QByteArray data = "ABCDEFGHIJ";

        ContentSummary summary(1, 0, "none", "none", "xxHash",
        {
            HashingStrategy::xxHash(data.mid(0, 1)),
            HashingStrategy::xxHash(data.mid(1, 1)),
            HashingStrategy::xxHash(data.mid(2, 1)),
            HashingStrategy::xxHash(data.mid(3, 1)),
            HashingStrategy::xxHash(data.mid(4, 1)),
            HashingStrategy::xxHash(data.mid(5, 1)),
            HashingStrategy::xxHash(data.mid(6, 1)),
            HashingStrategy::xxHash(data.mid(7, 1)),
            HashingStrategy::xxHash(data.mid(8, 1)),
            HashingStrategy::xxHash(data.mid(9, 1))
        },
        {});

        GIVEN("A mocked NAM wich replies to 'link' with the specified data in 300 ms.")
        {
            MockedNAM nam;

            nam.push("link", data, 300);

            ChunkedDownloader downloader(&nam, summary, &HashingStrategy::xxHash, 1000, token);

            THEN ("With a 1000 ms permitted timeout, the chunked download should succeed.")
            {
                QByteArray downloadedData = downloader.downloadFile("link", 1000);

                REQUIRE(downloadedData.toStdString() == data.toStdString());
            }

            THEN ("With a 200 ms permitted timeout, an exception should occur.")
            {
                bool timeoutCaught = false;

                try
                {
                    downloader.downloadFile("link", 200);
                }
                catch (TimeoutException&)
                {
                    timeoutCaught = true;
                }

                REQUIRE(timeoutCaught == true);
            }
        }

        GIVEN("A mocked NAM which always corrupts the first reply.")
        {
            MockedNAM nam;

            nam.push("link", data, 300);
            nam.corrupt();

            THEN ("With a 1000 ms permitted timeout and 1000 ms permitted stale download timeout, the chunked download should succeed.")
            {
                ChunkedDownloader downloader(&nam, summary, &HashingStrategy::xxHash, 1000, token);

                QByteArray downloadedData = downloader.downloadFile("link", 1000);

                REQUIRE(downloadedData.toStdString() == data.toStdString());
            }
        }
    }
}

SCENARIO("Testing the chunked downloader's stale download functionality.", "[chunked_downloader]")
{
    std::shared_ptr<CancellationTokenSource> tokenSource(new CancellationTokenSource());
    CancellationToken token(tokenSource);

    GIVEN("A constant data set in chunks with a content summary.")
    {
        const QByteArray data = "ABCDEFGHIJ";

        ContentSummary summary(1, 0, "none", "none", "xxHash",
        {
            HashingStrategy::xxHash(data.mid(0, 1)),
            HashingStrategy::xxHash(data.mid(1, 1)),
            HashingStrategy::xxHash(data.mid(2, 1)),
            HashingStrategy::xxHash(data.mid(3, 1)),
            HashingStrategy::xxHash(data.mid(4, 1)),
            HashingStrategy::xxHash(data.mid(5, 1)),
            HashingStrategy::xxHash(data.mid(6, 1)),
            HashingStrategy::xxHash(data.mid(7, 1)),
            HashingStrategy::xxHash(data.mid(8, 1)),
            HashingStrategy::xxHash(data.mid(9, 1))
        },
        {});

        GIVEN("A mocked NAM replying to 'link' always with an empty reply in 100 ms")
        {
            MockedNAM nam;

            nam.push("link", "", 100);

            GIVEN("A chunked downloader config permitting 500 ms stale download timeout.")
            {
                ChunkedDownloader downloader(&nam, summary, &HashingStrategy::xxHash, 500, token);

                THEN("When downloading with 300 ms timeout a stale download exception should occur.")
                {
                    bool check = false;
                    try
                    {
                        downloader.downloadFile("link", 300);
                    }
                    catch(StaleDownloadException&)
                    {
                        check = true;
                    }

                    REQUIRE(check == true);
                }
            }
        }
    }
}

SCENARIO("Testing the chunked downloader's stale download functionality with a corrupted data set.", "[chunked_downloader]")
{
    std::shared_ptr<CancellationTokenSource> tokenSource(new CancellationTokenSource());
    CancellationToken token(tokenSource);

    GIVEN("A corrupted data set in chunks with a content summary.")
    {
        const QByteArray data =         "ABCDEFGHIJ";
        const QByteArray corrupedData = "1234567890";

        ContentSummary summary(1, 0, "none", "none", "xxHash",
        {
            HashingStrategy::xxHash(data.mid(0, 1)),
            HashingStrategy::xxHash(data.mid(1, 1)),
            HashingStrategy::xxHash(data.mid(2, 1)),
            HashingStrategy::xxHash(data.mid(3, 1)),
            HashingStrategy::xxHash(data.mid(4, 1)),
            HashingStrategy::xxHash(data.mid(5, 1)),
            HashingStrategy::xxHash(data.mid(6, 1)),
            HashingStrategy::xxHash(data.mid(7, 1)),
            HashingStrategy::xxHash(data.mid(8, 1)),
            HashingStrategy::xxHash(data.mid(9, 1))
        },
        {});

        GIVEN("A mocked NAM replying to 'link' with a corruped data reply in 100 ms")
        {
            MockedNAM nam;

            nam.push("link", corrupedData, 100);

            GIVEN("A chunked downloader config permitting 1000 ms stale download timeout.")
            {
                ChunkedDownloader downloader(&nam, summary, &HashingStrategy::xxHash, 1000, token);

                THEN("When downloading with 300 ms timeout a stale download exception should occur.")
                {
                    bool check = false;
                    try
                    {
                        downloader.downloadFile("link", 300);
                    }
                    catch(StaleDownloadException&)
                    {
                        check = true;
                    }

                    REQUIRE(check == true);
                }
            }
        }
    }
}

SCENARIO("Simulating RemotePatcherData's alternate url's functionality.", "[chunked_downloader]")
{
    std::shared_ptr<CancellationTokenSource> tokenSource(new CancellationTokenSource());
    CancellationToken token(tokenSource);

    GIVEN("A configuration permitting 1000ms stale timeout, 100ms minimal timeout, 300ms max timeout.")
    {
        const QByteArray data =         "ABCDEFGHIJ";

        int staleDownloadTimeoutMsec = 1000;
        int minTimeoutMsec = 100;
        int maxTimeoutMsec = 300;

        ContentSummary summary(1, 0, "none", "none", "xxHash",
        {
            HashingStrategy::xxHash(data.mid(0, 1)),
            HashingStrategy::xxHash(data.mid(1, 1)),
            HashingStrategy::xxHash(data.mid(2, 1)),
            HashingStrategy::xxHash(data.mid(3, 1)),
            HashingStrategy::xxHash(data.mid(4, 1)),
            HashingStrategy::xxHash(data.mid(5, 1)),
            HashingStrategy::xxHash(data.mid(6, 1)),
            HashingStrategy::xxHash(data.mid(7, 1)),
            HashingStrategy::xxHash(data.mid(8, 1)),
            HashingStrategy::xxHash(data.mid(9, 1))
        },
        {});

        GIVEN("A mocked NAM with 2 possible responses, both valid, the first responds in 400ms, the second in 200ms.")
        {
            MockedNAM nam;
            nam.push("link1", data, 400);
            nam.push("link2", data, 200);

            ChunkedDownloader downloader(&nam, summary, HashingStrategy::xxHash, staleDownloadTimeoutMsec, token);

            int timeoutCount = 0;

            WHEN("Downloading from the first link, 2 timeouts should occur.")
            {
                try
                {
                    try
                    {
                        downloader.downloadFile("link1", minTimeoutMsec);
                    }
                    catch (TimeoutException&)
                    {
                        timeoutCount++;
                        downloader.downloadFile("link1", maxTimeoutMsec);
                    }
                }
                catch (TimeoutException&)
                {
                    timeoutCount++;
                }

                REQUIRE(timeoutCount == 2);
            }

            timeoutCount = 0;

            WHEN("Downloading from the second link, 1 timeout should occur and the download should succed the second time.")
            {
                QByteArray downloadedData;

                try
                {
                    try
                    {
                        downloader.downloadFile("link2", minTimeoutMsec);
                    }
                    catch (TimeoutException&)
                    {
                        timeoutCount++;
                        downloadedData = downloader.downloadFile("link2", maxTimeoutMsec);
                    }
                }
                catch (TimeoutException&)
                {
                    timeoutCount++;
                }

                CHECK(timeoutCount == 1);
                REQUIRE(downloadedData.toStdString() == data.toStdString());
            }
        }
    }
}
