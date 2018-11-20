#include "chunkedbuffer.h"

using namespace downloading::chunked;

ChunkedBuffer::ChunkedBuffer(const ChunkInfo& chunkInfo,
                             HashFunc hashingMethod,
                             QIODevice& target)
    : m_chunkInfo(chunkInfo)
    , m_hashingMethod(hashingMethod)
    , m_target(target)
{
    if (target.size() % chunkInfo.chunkSize() != 0)
    {
        throw ChunkVerificationException("Downloaded data is either complete or invalid.");
    }

    m_chunkIndex = static_cast<int>(target.size() / chunkInfo.chunkSize());
}

qint64 ChunkedBuffer::readData(char* /*data*/, qint64 /*maxSize*/)
{
    throw NotSupportedException("Reading from the chunked buffer is not supported.");
}

qint64 ChunkedBuffer::writeData(const char* data, qint64 maxSize)
{
    auto chunkSize = m_chunkInfo.chunkSize();
    auto written = m_buffer.write(data, maxSize);

    while (m_buffer.size() >= chunkSize)
    {
        QByteArray chunk = m_buffer.read(chunkSize);

        auto validHash = m_chunkInfo.chunkHash(m_chunkIndex);
        auto actualHash = m_hashingMethod(chunk);

        if (validHash != actualHash)
        {
            throw ChunkVerificationException("Chunk verification failed");
        }

        m_target.write(chunk);
        m_chunkIndex++;
    }

    return written;
}

ChunkInfo::ChunkInfo(const ContentSummary& summary)
{
    m_chunkSize = summary.getChunkSize();

    for (int i = 0; i < summary.getChunksCount(); i++)
    {
        m_hashes.push_back(summary.getChunkHash(i));
    }
}

ChunkInfo::ChunkInfo(int chunkSize, const std::vector<unsigned int>& hashes)
    : m_chunkSize(chunkSize)
    , m_hashes(hashes)
{
}

int ChunkInfo::chunkSize() const
{
    return m_chunkSize;
}

unsigned int ChunkInfo::chunkHash(int index) const
{
    return m_hashes.at(static_cast<size_t>(index));
}
