#pragma once

namespace data
{

    enum class NetworkStatus
    {
        Offline, Online
    };

    static constexpr const char* ToString(NetworkStatus status)
    {
        switch (status)
        {
            case NetworkStatus::Online:
                return "online";

            case NetworkStatus::Offline:
                return "offline";
        }
    }
} // namespace data

