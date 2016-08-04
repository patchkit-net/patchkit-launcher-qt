/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#ifndef LAUNCHERLOG_H
#define LAUNCHERLOG_H

#define logStringFormat(message, ...) QString(message)__VA_ARGS__.toStdString().c_str()

#define logDebug(message, ...) qDebug(logStringFormat(message, ##__VA_ARGS__))
#define logInfo(message, ...) qInfo(logStringFormat(message, ##__VA_ARGS__))
#define logWarning(message, ...) qWarning(logStringFormat(message, ##__VA_ARGS__))
#define logCritical(message, ...) qCritical(logStringFormat(message, ##__VA_ARGS__))

#endif // LAUNCHERLOG_H