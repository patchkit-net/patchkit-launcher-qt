/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#ifndef HASHINGSTRATEGY_H
#define HASHINGSTRATEGY_H

#include <QByteArray>

#define XXH_PRIVATE_API
#include "xxhash.h"

typedef unsigned int THash;

typedef THash (*HashFunc)(const QByteArray&);

namespace HashingStrategy
{
    THash xxHash(const QByteArray& t_data);
}

#endif // HASHINGSTRATEGY_H
