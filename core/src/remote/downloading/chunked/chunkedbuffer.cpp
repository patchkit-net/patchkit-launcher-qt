#include "chunkedbuffer.h"

using namespace downloading::chunked;

ChunkedBuffer::ChunkedBuffer(const QVector<THash> expectedHashes, int chunkSize, HashFunc hashingMethod, QIODevice& target)
    : m_expectedHashes(expectedHashes)
    , m_chunkSize(chunkSize)
    , m_hashingMethod(hashingMethod)
    , m_target(target)
    , m_chunkIndex(0)
{
}

ChunkedBuffer::~ChunkedBuffer()
{
}

int ChunkedBuffer::validChunksWritten() const
{
    return m_chunkIndex;
}

void ChunkedBuffer::close()
{
    processChunk(m_buffer);

    QIODevice::close();
}

qint64 ChunkedBuffer::readData(char* /*data*/, qint64 /*maxSize*/)
{
    throw NotSupportedException("Reading from the chunked buffer is not supported.");
}

qint64 ChunkedBuffer::writeData(const char* data, qint64 maxSize)
{
    qint64 oldSize = m_buffer.size();
    m_buffer += QByteArray(data, static_cast<int>(maxSize));

    qint64 written = m_buffer.size() - oldSize;

    if (m_buffer.size() >= m_chunkSize)
    {
        auto bufferSize = m_buffer.size();
        auto chunkCount = m_buffer.size() / m_chunkSize;

        for (int i = 0; i < chunkCount; i++)
        {
            processChunk(m_buffer.mid(i * m_chunkSize, m_chunkSize));
        }

        m_buffer = m_buffer.right(bufferSize - (chunkCount * m_chunkSize));
    }

    return written;
}

void ChunkedBuffer::processChunk(const QByteArray& chunk)
{
    auto chunkSize = chunk.size();

    if (chunkSize > m_chunkSize)
    {
        throw ChunkVerificationException("Chunk too big");
    }

    auto validHash = m_expectedHashes.at(m_chunkIndex);
    auto actualHash = m_hashingMethod(chunk);

    if (validHash != actualHash)
    {
        auto msg = QString("Failed to verify chunk #%1 expected hash %2 got %3")
                .arg(m_chunkIndex)
                .arg(QString::number(validHash, 16))
                .arg(QString::number(actualHash, 16));

        throw ChunkVerificationException(msg);
    }

    m_target.write(chunk);
    m_chunkIndex++;
}
