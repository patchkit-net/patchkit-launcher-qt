#pragma once
#include "customexceptions.h"

namespace data
{
    enum class NetworkStatus
    {
        Offline, Online
    };

    const char* ToString(NetworkStatus status);

} // namespace data

