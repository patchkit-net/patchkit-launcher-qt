#ifndef LAUNCHERCONFIGURATION_H
#define LAUNCHERCONFIGURATION_H

#include <QString>

struct LauncherConfiguration
{
public:
    LauncherConfiguration(const QString& t_dataFileName);

    const QString dataFileName;
};

#endif // LAUNCHERCONFIGURATION_H
