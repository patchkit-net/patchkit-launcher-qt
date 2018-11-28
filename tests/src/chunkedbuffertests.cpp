#include "catch.h"

#include <src/remote/downloading/chunked/chunkedbuffer.h>
#include <src/hashingstrategy.h>
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

TEST_CASE("Writing chunks")
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

    buffer.open(QIODevice::WriteOnly);

    INFO("Writing chunk data in parts");
    buffer.write(QByteArray("a"));
    buffer.write("b");

    CHECK(target.data().toStdString() == "ab");
    CHECK(buffer.validChunksWritten() == 1);

    INFO("Writing entire chunks at a time");
    buffer.write("cd");

    CHECK(target.data().toStdString() == "abcd");
    CHECK(buffer.validChunksWritten() == 2);

    INFO("Accounting for the last chunk by closing");
    buffer.write("efg");
    buffer.close();

    REQUIRE(target.data().toStdString() == "abcdefg");
    CHECK(buffer.validChunksWritten() == 4);
}

TEST_CASE("Reading from chunked buffer is not supported")
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
    buffer.open(QIODevice::ReadOnly);

    QByteArray buf;

    REQUIRE_THROWS_AS(buffer.read(buf.data(), 10), NotSupportedException);
}

TEST_CASE("Writing an invalid chunk throws an exception")
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
    buffer.open(QIODevice::WriteOnly);

    REQUIRE_THROWS_AS(buffer.write("12"), ChunkedBuffer::ChunkVerificationException);
}
