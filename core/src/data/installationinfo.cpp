#include "installationinfo.h"
#include "customexceptions.h"

#include <QDir>

InstallationInfo InstallationInfo::from(QIODevice& source)
{
    InstallationInfo info;
    QString line;

    while (!(line = source.readLine()).isNull())
    {
        info.add(line.chopped(1));
    }

    source.close();

    return info;
}

InstallationInfo::InstallationInfo()
{
}

QStringList InstallationInfo::installedFiles() const
{
    return m_installedFiles;
}

bool InstallationInfo::isInstalled(const QString& path) const
{
    return m_installedFiles.contains(path);
}

void InstallationInfo::saveTo(QIODevice& target) const
{
    if (!target.isOpen())
    {
        if (!target.open(QIODevice::WriteOnly))
        {
            throw FatalException("Failed to save installation info");
        }
    }

    for (QString entry : m_installedFiles)
    {
        target.write(entry.toUtf8() + "\n");
    }
}

void InstallationInfo::add(const QString& path)
{
    m_installedFiles.push_back(path);
}

void InstallationInfo::remove(const QString& path)
{
    m_installedFiles.removeOne(path);
}
