/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QString>

#define logStringFormat(message, ...) QString(message)__VA_ARGS__.toStdString().c_str()

#define logDebug(message, ...) qDebug(logStringFormat(message, ##__VA_ARGS__))
#define logInfo(message, ...) qInfo(logStringFormat(message, ##__VA_ARGS__))
#define logWarning(message, ...) qWarning(logStringFormat(message, ##__VA_ARGS__))
#define logCritical(message, ...) qCritical(logStringFormat(message, ##__VA_ARGS__))

class Logger
{
public:
    static void setupLogFile(const QString& t_logFilePath);

    static QString adjustSecretForLog(const QString& t_secret);
    static QString adjustSecretForLog(const char* t_secret);
};
