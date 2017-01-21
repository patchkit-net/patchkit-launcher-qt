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

            ChunkedDownloader downloader(&nam, summary, &HashingStrategy::xxHash, 1000);

            THEN ("With a 1000 ms permitted timeout, the chunked download should succeed.")
            {
                QByteArray downloadedData = downloader.downloadFile("link", 1000, token);

                REQUIRE(downloadedData.toStdString() == data.toStdString());
            }

            THEN ("With a 200 ms permitted timeout, an exception should occur.")
            {
                bool timeoutCaught = false;

                try
                {
                    downloader.downloadFile("link", 200, token);
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
            nam.corruptNext();

            THEN ("With a 1000 ms permitted timeout and 1000 ms permitted stale download timeout, the chunked download should succeed.")
            {
                ChunkedDownloader downloader(&nam, summary, &HashingStrategy::xxHash, 1000);

                QByteArray downloadedData = downloader.downloadFile("link", 1000, token);

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

            GIVEN("A chunked downloader config permitting 300 ms stale download timeout.")
            {
                ChunkedDownloader downloader(&nam, summary, &HashingStrategy::xxHash, 300);

                THEN("When downloading with 300 ms timeout an exception should occur.")
                {
                    bool check = false;
                    try
                    {
                        downloader.downloadFile("link", 300, token);
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
