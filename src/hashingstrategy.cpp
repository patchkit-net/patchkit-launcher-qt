/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "hashingstrategy.h"

THash HashingStrategy::xxHash(const QByteArray& data)
{
    return XXH32(data.data(), data.size(), 42);
}
