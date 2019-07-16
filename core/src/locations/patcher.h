#pragma once

#include <QString>

namespace locations
{
    class Patcher
    {
    public:
        Patcher(const QString& patcherDirectory);

        QString directory() const;
        QString installationInfoFile() const;
        QString versionInfoFile() const;

        /**
        * The id_info file path, the id_info file contains the version id
        * of the installed patcher
        */
        QString idInfoFile() const;

        QString manifestFile() const;

        /**
        * Path to the location of the file that should
        * contain the location of the launcher
        */
        QString launcherLocationFile() const;
    private:
        QString m_directory;
    };
} // namespace locations
