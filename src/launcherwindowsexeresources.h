#ifndef LAUNCHERWINDOWSEXERESOURCES_H
#define LAUNCHERWINDOWSEXERESOURCES_H

#include <QObject>
#include <memory>

#ifdef Q_OS_WIN

class LauncherWindowsExeResources
{
public:
    static std::shared_ptr<QByteArray> extract(const QString& t_executableName, int resource_id, int resource_type_id);
};

#endif

#endif // LAUNCHERWINDOWSEXERESOURCES_H
