/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include <QApplication>

#include <src/logger.h>

#include <src/hashingstrategy.h>

#include "catch.h"

TEST_CASE("HashingStrategy xxHash")
{
    QByteArray dataOne = "TestData";
    QByteArray dataTwo = "Dummy";

    REQUIRE(HashingStrategy::xxHash(dataOne) == HashingStrategy::xxHash(dataOne));
    REQUIRE(HashingStrategy::xxHash(dataOne) != HashingStrategy::xxHash(dataTwo));
    REQUIRE(HashingStrategy::xxHash(dataTwo) == HashingStrategy::xxHash(dataTwo));
}
