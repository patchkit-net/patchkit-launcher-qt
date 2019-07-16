/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QFile>
#include <QTextStream>
#include <QtDebug>

#include <memory>

class Logger
{
public:
    static void initialize(const QString& workingDir);
    Logger(Logger const&) = delete;
    void operator=(Logger const&) = delete;

    void setSilent(bool t_silent);

    static QString adjustSecretForLog(const QString& t_secret);
    static QString adjustSecretForLog(const char* t_secret);
    static Logger& getInstance();
    Logger(const QString& workingDir);
private:
    QFile m_logFile;
    QTextStream m_logFileStream;
    QTextStream m_stdoutStream;

    bool m_isSilent;

    static QString logFileLocation();

    static std::shared_ptr<Logger> instance;

    static const char* resolveMessageType(QtMsgType t_type);
    static void logHandler(QtMsgType t_type, const QMessageLogContext& t_context, const QString& t_msg);
};
