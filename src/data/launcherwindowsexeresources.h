/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>
#include <memory>

#ifdef Q_OS_WIN

class LauncherWindowsExeResources
{
public:
    static std::shared_ptr<QByteArray> extract(const QString& t_executableName, int resource_id, int resource_type_id);
};

#endif
