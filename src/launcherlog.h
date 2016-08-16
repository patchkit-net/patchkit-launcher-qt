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

inline QString safeSecretLog(const QString &secret)
{
    QString result = secret;
    result.chop(2);
    result = result.remove(0, 2);
    result = result.append("..");
    result = result.prepend("..");
    return result;
}

inline QString safeSecretLog(const char* secret)
{
    return safeSecretLog(QString(secret));
}
