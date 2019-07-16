#include "interface.h"

#include <customexceptions.h>

#include <QDebug>

Interface::Interface(QObject* parent)
    : QObject(parent)
{
}

ILauncherInterface::OfflineModeAnswer Interface::shoulStartInOfflineMode()
{
    qInfo() << "Asking if launcher should start in offline mode";
    OfflineModeAnswer ans;

    emit shouldStartInOfflineModeSignal(ans);

    qInfo() << "Answer was " << (int) ans;
    return ans;
}

bool Interface::shouldRetry(const QString& reason)
{
    qInfo() << "Asking if launcher should retry";

    bool ans;

    emit shouldRetrySignal(reason, ans);

    qInfo() << "Answer was " << ans;
    return ans;
}

void Interface::displayErrorMessage(const QString& msg)
{
    qInfo() << "Displaying error message";

    emit showErrorMessage(msg);
}

void Interface::selectInstallationLocation(QString& location, bool& shouldCancel)
{
    qInfo() << "Selecting installation location";

    emit selectInstallationLocationSignal(location, shouldCancel);
}

