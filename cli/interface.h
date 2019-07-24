#pragma once
#include <ilauncher.h>
#include <QObject>

class CliInterface : public QObject, public ILauncherInterface
{
    Q_OBJECT
public:
    explicit CliInterface(QObject* parent = nullptr);

    virtual ILauncherInterface::OfflineModeAnswer shoulStartInOfflineMode() override;
    virtual bool shouldRetry(const QString& reason) override;
    virtual void displayErrorMessage(const QString& msg) override;
};
