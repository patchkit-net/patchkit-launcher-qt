#pragma once

#include <QString>
#include "application.h"
#include "patcher.h"
#include "installation.h"

namespace locations
{
    QString workingDirectory(const QString& appSecret);
    QString locationFile(const QString& workingDir);
    QString dataFilePath();
    QString logFilePath(const QString& workingDir);
    QString launcherExecutable();

    class Launcher
    {
    public:
        static Launcher initalize(const QString& appSecret,  const Installation& installation);
        Launcher(const Launcher&) = default;

        const Patcher& patcher() const;
        const Application& application() const;

        QString directory() const;
        QString executable() const;

        QString lockFile() const;
        QString logFile() const;
        QString dataFile() const;
    private:
        Launcher(const QString& workingDir, const QString& patcherDir, const QString& appDir);

        QString m_workingDir;

        Patcher m_patcher;
        Application m_app;
    };
} // namespace locations
