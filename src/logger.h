/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QFile>
#include <QTextStream>

#define logStringFormat(message, ...) QString(message)__VA_ARGS__.toStdString().c_str()

#define logDebug(message, ...) qDebug(logStringFormat(message, ##__VA_ARGS__))
#define logInfo(message, ...) qInfo(logStringFormat(message, ##__VA_ARGS__))
#define logWarning(message, ...) qWarning(logStringFormat(message, ##__VA_ARGS__))
#define logCritical(message, ...) qCritical(logStringFormat(message, ##__VA_ARGS__))

class Logger
{
    Logger();
public:
    Logger(Logger const&) = delete;
    void operator=(Logger const&) = delete;

    static Logger& getInstance()
    {
        static Logger instance;

        return instance;
    }

    static void initialize()
    {
        getInstance();
    }

    static QString adjustSecretForLog(const QString& t_secret);
    static QString adjustSecretForLog(const char* t_secret);
private:
    QFile m_logFile;
    QTextStream m_logFileStream;
    QTextStream m_stdoutStream;

    static const char* resolveMessageType(QtMsgType t_type);
    static void logHandler(QtMsgType t_type, const QMessageLogContext& t_context, const QString& t_msg);
};
