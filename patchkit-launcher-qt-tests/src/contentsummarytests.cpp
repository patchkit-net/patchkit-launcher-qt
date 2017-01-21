/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "catch.h"

#include <QString>
#include <QJsonDocument>

#include <src/contentsummary.h>

#include <string>

const std::string contentSummaryData =
"{"
"  \"size\": 1234,"
"  \"encryption_method\": \"none\","
"  \"compression_method\": \"zip\","
"  \"hashing_method\" : \"xxhash\","
"  \"hash_code\" : \"123456\","
"  \"files\": ["
"    {"
"      \"path\": \"path/to/some_file\","
"      \"hash\": \"12345\","
"      \"size\": \"12345\","
"      \"flags\": \"ignore\""
"    }"
"  ],"
"  \"chunks\": {"
"     \"size\": 1048576,"
"     \"hashes\": [\"3\", \"1\", \"5\"]"
"   }\n"
"}";

TEST_CASE("ContentSummary testing on a const data source.", "content_summary")
{
    QString data = "";

    ContentSummary summary;

    CHECK(summary.isValid() == false);

    SECTION("Parsing from empty JSON doc, summary should be invalid")
    {
        QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());

        summary = ContentSummary(doc);

        REQUIRE(summary.isValid() == false);
    }

    SECTION("Parsing from a valid JSON doc.")
    {
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(contentSummaryData));

        summary = ContentSummary(doc);

        REQUIRE(summary.isValid() == true);

        SECTION("Validating parsed data.")
        {
            REQUIRE(summary.getHashingMethod().toStdString() == "xxhash");
            REQUIRE(summary.getCompressionMethod().toStdString() == "zip");
            REQUIRE(summary.getEncryptionMethod().toStdString() == "none");

            REQUIRE(summary.getChunksCount() == 3);
            REQUIRE(summary.getChunkHash(0) == 3);
            REQUIRE(summary.getChunkHash(1) == 1);
        }

        SECTION("Testing error checking abilites.")
        {
            bool outOfBounds;
            REQUIRE(summary.getChunkHash(-1, outOfBounds) == 0);
            REQUIRE(outOfBounds == true);

            REQUIRE(summary.getChunkHash(2, outOfBounds) == 5);
            REQUIRE(outOfBounds == false);
        }
    }
}
