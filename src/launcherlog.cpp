/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcherlog.h"
#include <qdatetime.h>
#include <QDir>

QString logFilePath;
QStringList logHistory;

const char *resolveMessageType(QtMsgType t_type)
{
    if (t_type == QtDebugMsg)
    {
        return "[DEBUG]    ";
    }
    else if (t_type == QtInfoMsg)
    {
        return "[INFO]     ";
    }
    else if (t_type == QtWarningMsg)
    {
        return "[WARNING]  ";
    }
    else if (t_type == QtCriticalMsg)
    {
        return "[CRITICAL] ";
    }
    else if (t_type == QtFatalMsg)
    {
        return "[FATAL]    ";
    }
    else if (t_type == QtSystemMsg)
    {
        return "[SYSTEM]   ";
    }

    return "[UNKNOWN]  ";
}

void logHandler(QtMsgType t_type, const QMessageLogContext& t_context, const QString& t_msg)
{
    QString txt = t_msg;
    txt.prepend(" - ");

    QDateTime date;
    txt.prepend(date.currentDateTime().toString());

    txt.prepend(resolveMessageType(t_type));

#ifdef QT_DEBUG
    txt.append(" (");
    txt.append(QFileInfo(t_context.file).fileName());
    txt.append(":");
    txt.append(QString::number(t_context.line));
    txt.append(")");
#endif

    logHistory << txt;

    QTextStream(stdout) << txt << endl;

    QFile logFile(logFilePath);

    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        QTextStream logStream(&logFile);
        logStream << txt << endl << flush;
        logFile.close();
    }
}

void LauncherLog::setupLogFile(const QString& t_logFilePath)
{
    static QString lastLogFilePath;
    static bool isLogInitialized = false;

    if(!isLogInitialized)
    {
        qInstallMessageHandler(logHandler);
        isLogInitialized = true;
    }

    logFilePath = t_logFilePath;

    if (!lastLogFilePath.isEmpty() && lastLogFilePath != logFilePath)
    {
        QFile(lastLogFilePath).remove();
    }

    QFile logFile(logFilePath);

    if (logFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        lastLogFilePath = logFilePath;

        QTextStream logStream(&logFile);
        for (int i = 0; i < logHistory.length(); i++)
        {
            logStream << logHistory[i] << endl;
        }
        logStream << flush;
        logFile.close();
    }
}

QString LauncherLog::adjustSecretForLog(const QString& t_secret)
{
    QString result = t_secret;
    result.chop(2);
    result = result.remove(0, 2);
    result = result.append("..");
    result = result.prepend("..");
    return result;
}

QString LauncherLog::adjustSecretForLog(const char* t_secret)
{
    return adjustSecretForLog(QString(t_secret));
}
