#pragma once
#include "src/ilauncher.h"

class Interface : public ILauncherInterface
{
public:
    Interface();

    virtual ILauncherInterface::OfflineModeAnswer shoulStartInOfflineMode() override;
    virtual bool shouldRetry(QString reason) override;
};
