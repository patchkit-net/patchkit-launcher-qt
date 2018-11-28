#pragma once

#include "../../../hashingstrategy.h"

#include "../../../customexceptions.h"

#include <QByteArray>
#include <QBuffer>
#include <QVector>

namespace downloading
{
namespace chunked
{
class ChunkedBuffer : public QIODevice
{
public:
    CUSTOM_RUNTIME_ERROR(ChunkVerificationException)

    ChunkedBuffer(
            const QVector<THash> expectedHashes,
            int chunkSize,
            HashFunc hashingMethod,
            QIODevice& target);

    ~ChunkedBuffer() override;

    int validChunksWritten() const;

    virtual void close() override;

protected:
    qint64 readData(char* data, qint64 maxSize) override;
    qint64 writeData(const char* data, qint64 maxSize) override;

private:
    void processChunk(const QByteArray& chunk);

    QVector<THash> m_expectedHashes;
    int m_chunkSize;

    HashFunc m_hashingMethod;
    QIODevice& m_target;

    QByteArray m_buffer;
    int m_chunkIndex;
};

} // namespace chunked
} // namespace downloading
