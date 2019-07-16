#include "launcher.h"

#include <QApplication>
#include <QDir>
#include <QStandardPaths>

#include "config.h"

//#define Q_OS_OSX 1

#if defined(Q_OS_OSX)
QDir osxWritableDirectory()
{
    auto sep = QDir::separator();

    auto genericDataLocation = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);

    auto appsDirectory = QDir(genericDataLocation + sep + "PatchKit" + sep + "Apps");

    if (!appsDirectory.exists())
    {
        appsDirectory.mkpath(".");
    }

    return appsDirectory;
}
#endif

QString locations::workingDirectory(const QString& appSecret)
{
    QDir baseDir = QApplication::applicationDirPath();

#if defined(Q_OS_OSX)
    baseDir = osxWritableDirectory().filePath(appSecret.mid(0, 8));
#endif

    if (!baseDir.exists())
    {
        baseDir.mkpath(".");
    }

    return baseDir.path();
}

locations::Launcher locations::Launcher::initalize(const QString& appSecret, const Installation& installation)
{
    QString workingDir = workingDirectory(appSecret);

    QString patcherDir = installation.patcherLocation();
    QString appDir = installation.appLocation();

    return Launcher(workingDir, patcherDir, appDir);
}

const locations::Patcher& locations::Launcher::patcher() const
{
    return m_patcher;
}

const locations::Application& locations::Launcher::application() const
{
    return m_app;
}

QString locations::Launcher::directory() const
{
    return m_workingDir;
}

QString locations::Launcher::executable() const
{
    return QApplication::applicationFilePath();
}

QString locations::Launcher::lockFile() const
{
    return QDir(directory()).filePath(Config::lockFileName);
}

QString locations::Launcher::logFile() const
{
    return QDir(directory()).filePath(Config::logFileName);
}

QString locations::Launcher::dataFile() const
{
    return QDir(directory()).filePath(Config::dataFileName);
}

locations::Launcher::Launcher(const QString& workingDir, const QString& patcherDir, const QString& appDir)
    : m_workingDir(workingDir)
    , m_patcher(patcherDir)
    , m_app(appDir)
{}

QString locations::launcherExecutable()
{
    return QApplication::applicationFilePath();
}

QString locations::locationFile(const QString& workingDir)
{
    return QDir(workingDir).filePath(Config::locationFileName);
}
