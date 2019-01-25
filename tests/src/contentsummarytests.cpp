/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "catch.h"

#include <QString>
#include <QJsonDocument>

#include <data/contentsummary.h>

#include <string>

const std::string contentSummaryData =
R"(
{
    "size": 1234,
    "uncompressed_size": 4321,
    "encryption_method": "none",
    "compression_method": "zip",
    "hashing_method": "xxhash",
    "hash_code": "123456",
    "files": [
        {
            "path": "path/to/some_file",
            "hash": "12345",
            "size": "12345",
            "flags": "ignore"
        }
    ],
    "chunks": {
        "size": 1048576,
        "hashes": ["3", "1", "5"]
    }
}
)";

TEST_CASE("ContentSummary testing on a const data source.", "content_summary")
{
    QString data = "";

    SECTION("Parsing from empty JSON doc, summary should be invalid")
    {
        QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());

        REQUIRE_THROWS_AS(ContentSummary summary(doc), InvalidFormatException);
    }

    SECTION("Parsing from a valid JSON doc.")
    {
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(contentSummaryData));

        ContentSummary summary = ContentSummary(doc);

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

TEST_CASE("Content summary can be serialized to JSON." ,"[content_summary]")
{
    ContentSummary originalSummary(
        10, 123, "none", "none", "xxhash",
        {123, 321, 23, 43},
        {FileData("path", 123), FileData("differentPath", 321)},
        123, 321
    );

    QJsonDocument serializedContentSummary = originalSummary.toJson();

    ContentSummary deserializedContentSummary(serializedContentSummary);

    CHECK(originalSummary.getChunkSize() == deserializedContentSummary.getChunkSize());
    CHECK(originalSummary.getHashCode() == deserializedContentSummary.getHashCode());
    CHECK(originalSummary.getEncryptionMethod() == deserializedContentSummary.getEncryptionMethod());
    CHECK(originalSummary.getCompressionMethod() == deserializedContentSummary.getCompressionMethod());
    CHECK(originalSummary.getHashingMethod() == deserializedContentSummary.getHashingMethod());
    CHECK(originalSummary.getCompressedSize() == deserializedContentSummary.getCompressedSize());
    CHECK(originalSummary.getUncompressedSize() == deserializedContentSummary.getUncompressedSize());

    REQUIRE(originalSummary.getChunksCount() == deserializedContentSummary.getChunksCount());
    for (int i = 0; i < originalSummary.getChunksCount(); i++)
    {
        REQUIRE(originalSummary.getChunkHash(i) == deserializedContentSummary.getChunkHash(i));
    }

    REQUIRE(originalSummary.getFilesCount() == deserializedContentSummary.getFilesCount());
    for (int i = 0; i < originalSummary.getFilesCount(); i++)
    {
        const FileData& a = originalSummary.getFileData(i);
        const FileData& b = deserializedContentSummary.getFileData(i);

        REQUIRE(a.hash == b.hash);
        REQUIRE(a.path == b.path);
    }
}

TEST_CASE("Generating content summary from data.", "[content_summary]")
{
    int chunkSize = 2;
    int compressedSize = 123;
    int uncompressedSize = 321;
    QByteArray data = "somerandomdata";

    ContentSummary contentSummary = ContentSummary::fromData(
                data, chunkSize, compressedSize, uncompressedSize,
                &HashingStrategy::xxHash);

    REQUIRE(contentSummary.getChunkSize() == chunkSize);
    REQUIRE(contentSummary.getChunksCount() == (data.size() / chunkSize));
    REQUIRE(contentSummary.getChunkHash(0) == HashingStrategy::xxHash(data.mid(0, chunkSize)));
    REQUIRE(contentSummary.getCompressedSize() == compressedSize);
    REQUIRE(contentSummary.getUncompressedSize() == uncompressedSize);
}
