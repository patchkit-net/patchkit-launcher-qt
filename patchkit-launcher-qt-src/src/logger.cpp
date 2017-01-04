/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "logger.h"

#include <QString>
#include <QDateTime>
#include <QTextStream>
#include "locations.h"

Logger::Logger() :
    m_logFile(Locations::getInstance().logFilePath()),
    m_logFileStream(&m_logFile),
    m_stdoutStream(stdout)
{
    m_logFile.open(QIODevice::WriteOnly | QIODevice::Truncate);

    qInstallMessageHandler(logHandler);
}

QString Logger::adjustSecretForLog(const QString& t_secret)
{
    QString result = t_secret;
    result.chop(2);
    result = result.remove(0, 2);
    result = result.append("..");
    result = result.prepend("..");
    return result;
}

QString Logger::adjustSecretForLog(const char* t_secret)
{
    return adjustSecretForLog(QString(t_secret));
}

const char* Logger::resolveMessageType(QtMsgType t_type)
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

void Logger::logHandler(QtMsgType t_type, const QMessageLogContext& t_context, const QString& t_msg)
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

    getInstance().m_stdoutStream << txt << endl << flush;
    getInstance().m_logFileStream << txt << endl << flush;
}
