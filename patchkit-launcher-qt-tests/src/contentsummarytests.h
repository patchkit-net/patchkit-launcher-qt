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
"      \"path\": \"path/to/added_file\","
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

TEST_CASE("ContentSummary parsing")
{
    QString data = "";

    ContentSummary summary;

    REQUIRE(summary.isValid() == false);

    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());

    summary = ContentSummary(doc);

    REQUIRE(summary.isValid() == false);

    doc = QJsonDocument::fromJson(QByteArray::fromStdString(contentSummaryData));

    summary = ContentSummary(doc);

    REQUIRE(summary.isValid() == true);

    REQUIRE(summary.getHashingMethod().toStdString() == "xxhash");
    REQUIRE(summary.getCompressionMethod().toStdString() == "zip");
    REQUIRE(summary.getEncryptionMethod().toStdString() == "none");

    REQUIRE(summary.getChunksCount() == 3);
    REQUIRE(summary.getChunkHash(0) == 3);
    REQUIRE(summary.getChunkHash(1) == 1);

    bool ok;
    REQUIRE(summary.getChunkHash(-1, ok) == 0);
    REQUIRE(ok == true);

    REQUIRE(summary.getChunkHash(2, ok) == 5);
    REQUIRE(ok == false);
}
