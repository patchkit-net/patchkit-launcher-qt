/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "hashingstrategy.h"

THash HashingStrategy::xxHash(const QByteArray& t_data)
{
    return XXH32(t_data.data(), t_data.size(), xxHashSeed);
}
