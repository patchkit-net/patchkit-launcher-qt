#ifndef HASHINGSTRATEGY_H
#define HASHINGSTRATEGY_H

#include <QByteArray>

#define XXH_PRIVATE_API
#include "xxhash.h"

typedef unsigned int THash;

typedef THash (*HashFunc)(const QByteArray& bytes);

namespace HashingStrategy
{
    THash xxHash(const QByteArray& data);
}

#endif // HASHINGSTRATEGY_H
