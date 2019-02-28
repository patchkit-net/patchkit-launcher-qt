#pragma once
#include <ilauncher.h>
#include <QObject>

class Interface : public QObject, public ILauncherInterface
{
    Q_OBJECT
public:
    explicit Interface(QObject* parent = nullptr);

    virtual ILauncherInterface::OfflineModeAnswer shoulStartInOfflineMode() override;
    virtual bool shouldRetry(const QString& reason) override;

signals:
    void shouldStartInOfflineModeSignal(
            ILauncherInterface::OfflineModeAnswer& offlineModeAns);

    void shouldRetrySignal(const QString& reason, bool& ans);
};
