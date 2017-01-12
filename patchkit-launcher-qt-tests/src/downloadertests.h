/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "catch.h"

#include <QtNetwork>

#include <QApplication>
#include "src/downloader.h"

#include "mockednam.h"

SCENARIO("Testing downloader capabilities.")
{
    GIVEN("A mocked NAM replying always with 'This is a test' string.")
    {
        QByteArray testData = "This is a test.";
        MockedNAM nam(testData);

        Downloader downloader(&nam);

        std::shared_ptr<CancellationTokenSource> tokenSource(new CancellationTokenSource());
        CancellationToken token(tokenSource);

        THEN("Downloader should download the specified string.")
        {
            QByteArray data = downloader.downloadFile("", 1000, token);

            REQUIRE(data.toStdString() == testData.toStdString());
        }
    }
}
