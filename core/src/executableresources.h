/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>
#include <memory>


class ExecutableResources
{
public:
#ifdef Q_OS_WIN
    static std::shared_ptr<QByteArray> extract(const QString& t_executableName, int t_resourceId, int t_resourceTypeId);
#endif
};
