#pragma once
#include "customexceptions.h"

namespace data
{

    enum class NetworkStatus
    {
        Offline, Online
    };

    static const char* ToString(NetworkStatus status)
    {
        switch (status)
        {
            case NetworkStatus::Online:
                return "online";

            case NetworkStatus::Offline:
                return "offline";

            default:
                throw NotSupportedException("NetworkStatus must be either offline or online");
        }
    }
} // namespace data

