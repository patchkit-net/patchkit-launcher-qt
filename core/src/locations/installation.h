#pragma once

#include <QString>

namespace locations
{
    class Installation
    {
    public:
        Installation();
        Installation(const QString& patcherInstallationLocation, const QString& appInstallationLocation);

        QString patcherLocation() const;
        QString appLocation() const;
        void save(const QString& path) const;

        static Installation load(const QString& workingDir);
        static bool canLoad(const QString& workingDir);
        static void save(const Installation& installation, const QString& path);
    private:
        QString m_patcherInstallationLocation;
        QString m_appInstallationLocation;
    };
} // namespace locations
