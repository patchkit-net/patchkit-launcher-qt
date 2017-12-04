/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "catch.h"

#include <QtNetwork>

#include <QApplication>
#include "src/downloader.h"

#include "mockednam.h"
#include "src/customexceptions.h"

SCENARIO("Downloader life.")
{
    MockedNAM nam;

    QByteArray originalData = "somedata";
    nam.push("link", originalData, 300);

    CancellationTokenSource tokenSource;
    CancellationToken token(tokenSource);

    Downloader d("link", &nam, token);

    REQUIRE(d.readData().toStdString() == QByteArray().toStdString());

    REQUIRE(d.wasStarted()  == false);
    REQUIRE(d.isRunning()   == false);
    REQUIRE(d.isFinished()  == false);

    d.start();

    REQUIRE(d.wasStarted()  == true);
    REQUIRE(d.isRunning()   == false);
    REQUIRE(d.isFinished()  == false);

    d.waitUntilFinished();

    REQUIRE(d.wasStarted()  == true);
    REQUIRE(d.isRunning()   == false);
    REQUIRE(d.isFinished()  == true);
    REQUIRE(d.encounteredAnError() == false);

    QByteArray data = d.readData();

    REQUIRE(data.toStdString() == originalData.toStdString());
    REQUIRE(d.getStatusCode() == 200);
}

SCENARIO("Downloading partial data.")
{
    MockedNAM nam;

    QByteArray originalData = "123456789";
    int rangeOffset = 3;

    nam.push("link", originalData, 300);

    CancellationTokenSource tokenSource;
    CancellationToken token(tokenSource);

    Downloader d("link", &nam, token);
    d.setRange(rangeOffset);

    d.start();

    d.waitUntilFinished();

    QByteArray data = d.readData();

    REQUIRE(data.toStdString() == originalData.mid(rangeOffset).toStdString());
}
