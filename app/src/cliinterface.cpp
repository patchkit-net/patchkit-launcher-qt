#include "cliinterface.h"

#include <customexceptions.h>

#include <QDebug>

CliInterface::CliInterface(
        const QString& installationLocation,
        ILauncherInterface::OfflineModeAnswer offlineModeAnswer,
        bool shouldRetryAnswer,
        QObject* parent)
    : QObject(parent)
    , m_installationLocation(installationLocation)
    , m_offlineModeAnswer(offlineModeAnswer)
    , m_shouldRetryAnswer(shouldRetryAnswer)
{
}

ILauncherInterface::OfflineModeAnswer CliInterface::shoulStartInOfflineMode()
{
    return m_offlineModeAnswer;
}

bool CliInterface::shouldRetry(const QString& /*reason*/)
{
    return m_shouldRetryAnswer;
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

