#include "interface.h"

#include <src/customexceptions.h>

#include <QMessageBox>
#include <QDebug>

Interface::Interface()
{
}

ILauncherInterface::OfflineModeAnswer Interface::shoulStartInOfflineMode()
{
    int answer = QMessageBox::question(nullptr, "Offline mode?",
        "Launcher had some trouble updating. Would you like to continue in offline mode?",
        QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);

    switch (answer)
    {
    case QMessageBox::Yes:
        return OfflineModeAnswer::YES;

    case QMessageBox::No:
        return OfflineModeAnswer::NO;

    case QMessageBox::Cancel:
        return OfflineModeAnswer::CANCEL;

    default:
        return OfflineModeAnswer::NO;
    }
}

bool Interface::shouldRetry(QString reason)
{
    QString message = QString("Error: %1").arg(reason);
    int answer = QMessageBox::critical(nullptr, "Error",
        message,
        QMessageBox::Yes, QMessageBox::No);

    return answer == QMessageBox::Yes;
}

