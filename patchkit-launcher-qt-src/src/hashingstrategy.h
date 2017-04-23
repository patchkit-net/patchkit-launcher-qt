/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QByteArray>

#define XXH_PRIVATE_API
#include "xxhash.h"

typedef unsigned int THash;

typedef THash (*HashFunc)(const QByteArray&);

namespace HashingStrategy
{
    const int xxHashSeed = 42;

    THash xxHash(const QByteArray& t_data);
}
