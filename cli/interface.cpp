#include "interface.h"

#include <customexceptions.h>

#include <QDebug>

CliInterface::CliInterface(const QString& installationLocation, QObject* parent)
    : QObject(parent)
    , m_installationLocation(installationLocation)
{
}

ILauncherInterface::OfflineModeAnswer CliInterface::shoulStartInOfflineMode()
{
    return ILauncherInterface::OfflineModeAnswer::CANCEL;
}

bool CliInterface::shouldRetry(const QString& /*reason*/)
{
    return false;
}

void CliInterface::displayErrorMessage(const QString& msg)
{
    qInfo() << "Displaying error message";
    qCritical() << msg;
}

void CliInterface::selectInstallationLocation(QString& location, bool& shouldCancel)
{
    location = m_installationLocation;
    shouldCancel = false;
}

