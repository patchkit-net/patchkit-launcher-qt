#include "interface.h"

#include <customexceptions.h>

#include <QDebug>

CliInterface::CliInterface(QObject* parent)
    : QObject(parent)
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

