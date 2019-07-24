#pragma once
#include <ilauncher.h>
#include <QObject>

class CliInterface : public QObject, public ILauncherInterface
{
    Q_OBJECT
public:
    explicit CliInterface(const QString& installationLocation, QObject* parent = nullptr);

    virtual ILauncherInterface::OfflineModeAnswer shoulStartInOfflineMode() override;
    virtual bool shouldRetry(const QString& reason) override;
    virtual void displayErrorMessage(const QString& msg) override;
    virtual void selectInstallationLocation(
            QString& location,
            bool& shouldCancel) override;

private:
    QString m_installationLocation;
};
