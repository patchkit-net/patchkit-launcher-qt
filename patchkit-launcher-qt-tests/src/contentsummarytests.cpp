/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "catch.h"

#include <QString>
#include <QJsonDocument>

#include <src/contentsummary.h>

TEST_CASE("ContentSummary parsing")
{
    QString data = "";

    ContentSummary summary;

    REQUIRE(summary.isValid() == false);

    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());

    summary = ContentSummary(doc);

    REQUIRE(summary.isValid() == false);
}
