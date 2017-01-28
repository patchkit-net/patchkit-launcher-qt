/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "catch.h"

#include <QApplication>

#include <src/logger.h>

#include <src/hashingstrategy.h>

TEST_CASE("HashingStrategy xxHash", "[xxHash]")
{
    QByteArray dataOne = "TestData";
    QByteArray dataTwo = "Dummy";

    REQUIRE(HashingStrategy::xxHash(dataOne) == HashingStrategy::xxHash(dataOne));
    REQUIRE(HashingStrategy::xxHash(dataOne) != HashingStrategy::xxHash(dataTwo));
    REQUIRE(HashingStrategy::xxHash(dataTwo) == HashingStrategy::xxHash(dataTwo));
}
