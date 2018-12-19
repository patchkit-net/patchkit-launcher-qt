#pragma once
#include <QString>

class ILauncherInterface
{
public:
    enum class OfflineModeAnswer
    {
        YES, NO, CANCEL
    };

    virtual ~ILauncherInterface() {}

    virtual OfflineModeAnswer shoulStartInOfflineMode() = 0;
    virtual bool shouldRetry(const QString& reason) = 0;
};
