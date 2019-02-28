#include "catch.h"

#include <remote/downloading/chunked/chunkedbuffer.h>
#include <hashingstrategy.h>
#include "custommacros.h"

#include <QBuffer>
#include <QVector>
#include <QIODevice>

namespace
{
    QVector<THash> hashData(QByteArray data, int chunkSize, HashFunc hasher)
    {
        QVector<THash> hashes;

        for (int i = 0; i < data.size(); i += chunkSize)
        {
            auto chunk = data.mid(i, chunkSize);
            auto hash = hasher(chunk);
            hashes.append(hash);
        }

        return hashes;
    }
}

SCENARIO("Expected chunked buffer behaviour")
{
    GIVEN("Some data with hashes of each chunk and a chunked buffer")
    {
        using namespace downloading::chunked;
        QByteArray originalData = "abcdefg";

        const int chunkSize = 2;
        QVector<THash> hashes
                = hashData(originalData, chunkSize, HashingStrategy::xxHash);

        QBuffer target;
        target.open(QIODevice::WriteOnly);

        ChunkedBuffer buffer(
                    hashes, chunkSize,
                    HashingStrategy::xxHash, target);

        WHEN("Correct data is written in unequal parts")
        {
            buffer.open(QIODevice::WriteOnly);
            buffer.write("a");
            buffer.write("b");

            buffer.write("cd");

            buffer.write("efg");
            buffer.close();

            THEN("The buffer should succesfully pass the data and count written chunks")
            {
                REQUIRE(buffer.validChunksWritten() == 4);
                REQUIRE(target.data().toStdString() == originalData.toStdString());
            }
        }

        WHEN("Correct data is written all at once")
        {
            buffer.open(QIODevice::WriteOnly);
            buffer.write(originalData);
            buffer.close();

            THEN("The buffer should succesfully pass the data and count written chunks")
            {
                REQUIRE(buffer.validChunksWritten() == 4);
                REQUIRE(target.data().toStdString() == originalData.toStdString());
            }
        }

        WHEN("Attempting to read from the buffer")
        {
            buffer.open(QIODevice::ReadOnly);
            THEN("An exception should be thrown")
            {
                REQUIRE_THROWS_AS(buffer.readAll(), NotSupportedException);
            }
        }

        WHEN("Writing invalid data into the buffer")
        {
            buffer.open(QIODevice::WriteOnly);
            buffer.write("abc");

            THEN("An exception should be thrown and the buffer should account for previously written chunks and discard all other data")
            {
                REQUIRE_THROWS_AS(buffer.write("12"), ChunkedBuffer::ChunkVerificationException);
                REQUIRE_THROWS_AS(buffer.write("dc"), ChunkedBuffer::ChunkVerificationException);
                REQUIRE(buffer.validChunksWritten() == 1);

                AND_WHEN("Writing correct data after that")
                {
                    buffer.write("cd");
                    buffer.write("efg");
                    buffer.close();
                    THEN("The buffer should succesfully pass the data and count written chunks")
                    {
                        REQUIRE(buffer.validChunksWritten() == 4);
                        REQUIRE(target.data().toStdString() == originalData.toStdString());
                    }
                }
            }
        }
    }
}
