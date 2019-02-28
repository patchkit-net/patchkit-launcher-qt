#pragma once
#include <QString>
#include <QStringList>
#include <QIODevice>

class InstallationInfo
{
public:
    static InstallationInfo from(QIODevice& source);

    InstallationInfo();

    QStringList installedFiles() const;
    bool isInstalled(const QString& path) const;

    void saveTo(QIODevice& target) const;

    void add(const QString& path);
    void remove(const QString& path);
private:
    QStringList m_installedFiles;
};
