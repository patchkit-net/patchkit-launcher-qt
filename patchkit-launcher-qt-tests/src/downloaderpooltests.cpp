/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "catch.h"

#include <QtNetwork>

#include <QApplication>
#include "src/downloaderoperator.h"
#include "src/defaultdownloadstrategy.h"

#include "mockednam.h"

TEST_CASE("Downloader pool ownership tests.")
{
    CancellationTokenSource tokenSource;
    MockedNAM nam;
    StringUrlProvider urlProvider({"one", "two", "three"});

    DownloaderPool mainPool(&nam, urlProvider, tokenSource);

    REQUIRE(mainPool.poolSize() == 3);
    REQUIRE(mainPool.hasOwnership() == true);

    DownloaderPool copyPool(mainPool);

    REQUIRE(copyPool.poolSize() == 3);
    REQUIRE(copyPool.hasOwnership() == false);

    DownloaderPool vectorInitiatedPool(mainPool.getDownloaders());

    REQUIRE(vectorInitiatedPool.poolSize() == 3);
    REQUIRE(vectorInitiatedPool.hasOwnership() == false);

    DownloaderPool listInitializedPool({mainPool.getDownloaders().at(0)});

    REQUIRE(listInitializedPool.poolSize() == 1);
    REQUIRE(listInitializedPool.hasOwnership() == false);
}
