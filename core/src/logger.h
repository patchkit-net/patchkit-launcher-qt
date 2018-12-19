/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QFile>
#include <QTextStream>
#include <QtDebug>

class Logger
{
    Logger();
public:
    Logger(Logger const&) = delete;
    void operator=(Logger const&) = delete;

    static Logger& getInstance();

    static void initialize();

    void setSilent(bool t_silent);

    static QString adjustSecretForLog(const QString& t_secret);
    static QString adjustSecretForLog(const char* t_secret);
private:
    QFile m_logFile;
    QTextStream m_logFileStream;
    QTextStream m_stdoutStream;

    bool m_isSilent;

    static QString logFileLocation();

    static const char* resolveMessageType(QtMsgType t_type);
    static void logHandler(QtMsgType t_type, const QMessageLogContext& t_context, const QString& t_msg);
};
