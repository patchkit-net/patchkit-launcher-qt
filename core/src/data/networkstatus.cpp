#include "networkstatus.hpp"

namespace data
{
const char* ToString(data::NetworkStatus status)
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
}
