#pragma once
#include <QString>
#include <QStringList>

class InstallationInfo
{
public:
    InstallationInfo();
    InstallationInfo(QString installationInfoFile);

    QStringList installedFiles() const;
    bool isInstalled(const QString& path) const;

    void add(const QString& path);
    void remove(const QString& path);
private:
    QStringList m_installedFiles;
};
