#pragma once

#include <QString>

namespace utils {
    namespace locations {
        // These are ALWAYS the same no matter what happens
        QString launcherExecutablePath();
        QString launcherDirectoryPath();

        // Depends on platform and installation setting
        // QString workingDirectory();

        // Depends on working dir (platform only)
        QString logFilePath(const QString& workingDir);

        // Always the same
        QString dataFilePath();

        // The path in which the patcher resides
        // Depends on the platform, installation location
        QString patcherDirectoryPath();






        /*
         * locations::Launcher launcherLocations("app-secret");
         *
         * launcherLocations.patcher().versionInfoPath();
         * launcherLocations.application().directory();
         */
    }
}
