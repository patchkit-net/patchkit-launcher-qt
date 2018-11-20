#pragma once

#include "data/contentsummary.h"
#include "hashingstrategy.h"

#include "customexceptions.h"

#include <QByteArray>
#include <QBuffer>

namespace downloading
{
namespace chunked
{

class ChunkInfo
{
public:
    ChunkInfo(const ChunkInfo&) = default;
    ChunkInfo(const ContentSummary& summary);
    ChunkInfo(int chunkSize, const std::vector<unsigned int>& hashes);

    int chunkSize() const;
    unsigned int chunkHash(int index) const;

private:
    int m_chunkSize;
    std::vector<unsigned int> m_hashes;
};

class ChunkedBuffer : public QIODevice
{
public:
    CUSTOM_RUNTIME_ERROR(ChunkVerificationException)

    ChunkedBuffer(const ChunkInfo& chunkInfo,
                  HashFunc hashingMethod,
                  QIODevice& target);

protected:
    qint64 readData(char* data, qint64 maxSize) override;
    qint64 writeData(const char* data, qint64 maxSize) override;

private:
    ChunkInfo m_chunkInfo;
    HashFunc m_hashingMethod;
    QIODevice& m_target;

    QBuffer m_buffer;
    int m_chunkIndex;
};

} // namespace chunked
} // namespace downloading
