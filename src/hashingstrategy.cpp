#include "hashingstrategy.h"

THash HashingStrategy::xxHash(const QByteArray& data)
{
    return XXH32(data.data(), data.size(), 42);
}
