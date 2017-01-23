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

SCENARIO("Testing downloader capabilities.")
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
