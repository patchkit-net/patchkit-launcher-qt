/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "logger.h"

#include <QString>
#include <QDateTime>
#include <QTextStream>
#include <QFileInfo>
#include "locations/launcher.h"

std::shared_ptr<Logger> Logger::instance = std::make_shared<Logger>(nullptr);

Logger::Logger(const QString& workingDir)
    : m_logFile(locations::logFilePath(workingDir))
    , m_logFileStream(&m_logFile)
    , m_stdoutStream(stdout)
    , m_isSilent(false)
{
    m_logFile.open(QIODevice::WriteOnly | QIODevice::Append);

    qInstallMessageHandler(logHandler);

    // Initial log message
    m_logFileStream << endl
                    << "########## "
                    << QDateTime::currentDateTime().toString()
                    << " - Log start"
                    << endl
                    << flush;
}

void Logger::initialize(const QString& workingDir)
{
    instance = std::make_shared<Logger>(workingDir);
}

void Logger::setSilent(bool t_silent)
{
    m_isSilent = t_silent;
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

Logger& Logger::getInstance()
{
    return *instance;
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
    if (!instance || instance->m_isSilent)
    {
        return;
    }

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

    instance->m_stdoutStream << txt << endl << flush;
    instance->m_logFileStream << txt << endl << flush;
}
