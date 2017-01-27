/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "catch.h"

#include <QtNetwork>

#include <QApplication>
#include "src/downloader.h"

#include "mockednam.h"
#include "src/timeoutexception.h"

SCENARIO("Testing downloader capabilities.", "[downloader]")
{
    GIVEN("A mocked NAM replying to 'link' url 'This is a test' string in 300 milliseconds.")
    {
        QByteArray testData = "This is a test.";
        MockedNAM nam;

        nam.push("link", testData, 300);

        std::shared_ptr<CancellationTokenSource> tokenSource(new CancellationTokenSource());
        CancellationToken token(tokenSource);

        Downloader downloader(&nam, token);

        int timeoutMsec = 0;

        GIVEN("Downloader configuration permitting a 1000 millisecond timeout.")
        {
            timeoutMsec = 1000;
            THEN("Downloader should download the string without errors.")
            {
                QByteArray data = downloader.downloadFile("link", timeoutMsec);

                REQUIRE(data.toStdString() == testData.toStdString());
            }
        }

        GIVEN("Downloader configuration permitting a 200 millisecond timeout.")
        {
            timeoutMsec = 200;
            THEN("A timeout exception should occur.")
            {
                bool timeoutCaught = false;
                try
                {
                    downloader.downloadFile("link", timeoutMsec);
                }
                catch (TimeoutException&)
                {
                    timeoutCaught = true;
                }

                REQUIRE(timeoutCaught == true);
            }
        }
    }
}

SCENARIO("Simulating RemotePatcherData's alternate link and min/max timeout functionality.", "[downloader]")
{
    std::shared_ptr<CancellationTokenSource> tokenSource(new CancellationTokenSource());
    CancellationToken token(tokenSource);

    const QByteArray data = "123456789";

    int minTimeout = 100;
    int maxTimeout = 300;

    GIVEN("A mocked NAM with two possible replies, both with valid data responding in respectively 200 and 400 ms.")
    {
        MockedNAM nam;
        nam.push("link1", data, 400);
        nam.push("link2", data, 200);

        Downloader downloader(&nam, token);

        WHEN("Downloading from the first link, 2 timeout exceptions should occur.")
        {
            int timeoutCounter = 0;

            try
            {
                try
                {
                    downloader.downloadFile("link1", minTimeout);
                }
                catch(TimeoutException&)
                {
                    ++timeoutCounter;
                    downloader.downloadFile("link1", maxTimeout);
                }
            }
            catch(TimeoutException&)
            {
                ++timeoutCounter;
            }

            REQUIRE(timeoutCounter == 2);
        }

        WHEN("Downloading from the second link, 1 timeout should occur and on the second try the download should succeed.")
        {
            int timeoutCounter = 0;

            QByteArray downloadedData;

            try
            {
                try
                {
                    downloadedData = downloader.downloadFile("link2", minTimeout);
                }
                catch(TimeoutException&)
                {
                    ++timeoutCounter;
                    downloadedData = downloader.downloadFile("link2", maxTimeout);
                }
            }
            catch(TimeoutException&)
            {
                ++timeoutCounter;
            }

            REQUIRE(timeoutCounter == 1);
            REQUIRE(downloadedData.toStdString() == data.toStdString());
        }
    }
}
