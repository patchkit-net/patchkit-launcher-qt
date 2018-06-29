/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "catch.h"

#include <QtNetwork>

#include <QApplication>
#include "src/downloaderoperator.h"
#include "src/defaultdownloadstrategy.h"

#include "mocks/nam.h"

SCENARIO("Operator state testing.")
{
    CancellationTokenSource tokenSource;
    CancellationToken token(tokenSource);
    MockedNAM nam;
    GIVEN("URL provider with 3 links.")
    {
        StringUrlProvider urlProvider({"l1", "l2", "l3"});

        DownloaderOperator op(&nam, (IUrlProvider&) urlProvider, token);

        THEN("The total number of downloaders should equal 3.")
        {
            REQUIRE(op.getDownloaders().size() == 3);
        }

        THEN("All downloaders should be inactive.")
        {
            REQUIRE(op.getDownloaders().size() == op.getInactiveDownloaders().size());
        }

        GIVEN("That all inactive downloaders have been started.")
        {
            for (Downloader* d : op.getInactiveDownloaders())
            {
                d->start();
            }

            THEN("The all downloaders should be starting.")
            {
                REQUIRE(op.getDownloaders().size() == op.getStartingDownloaders().size());
            }

            THEN("Upon stopping all the downloaders, all the downloaders should be inactive.")
            {
                op.stopAll();
                REQUIRE(op.getDownloaders().size() == op.getInactiveDownloaders().size());
            }
        }
    }
}

SCENARIO("Basic operator functionality with default download startegy.")
{
    CancellationTokenSource tokenSource;
    CancellationToken token(tokenSource);

    QByteArray data = "123456789";

    MockedNAM nam;

    GIVEN("A mocked NAM with 3 links worth of data, each responding in 100 msecs.")
    {
        nam.purge();
        nam.push("link1", data, 100);
        nam.push("link2", data, 100);
        nam.push("link3", data, 100);

        THEN("A DefaultDownloadStrategy with min and max timeouts set to 300 and 400 should succeed.")
        {
            StringUrlProvider urlProvider({"link1", "link2", "link3"});
            DownloaderOperator op(&nam, (IUrlProvider&) urlProvider, token);

            DefaultDownloadStrategy strategy(op, 300, 400);

            auto result = op.download(strategy, token);
            REQUIRE(200 == result.statusCode);
            REQUIRE(data.toStdString() == result.data.toStdString());
        }
    }

    GIVEN("A mocked nam with 3 links worth of data, the first responds in 1000 ms and the other 2 in 100 ms")
    {
        nam.purge();
        nam.push("link1", data, 1000);
        nam.push("link2", data, 100);
        nam.push("link3", data, 100);

        THEN("A DefaultDownloadStrategy with min and max timeouts set to 300 and 400 should succeed.")
        {
            StringUrlProvider urlProvider({"link1", "link2", "link3"});
            DownloaderOperator op(&nam, (IUrlProvider&) urlProvider, token);

            DefaultDownloadStrategy strategy(op, 300, 400);

            auto result = op.download(strategy, token);
            REQUIRE(200 == result.statusCode);
            REQUIRE(data.toStdString() == result.data.toStdString());
        }
    }
}
