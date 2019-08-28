/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "localpatcherdata.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QTextStream>
#include <QDir>
#include <QFile>

#include "config.h"
#include "logger.h"
#include "ioutils.h"
#include "lockfile.h"
#include "customexceptions.h"
#include "utilities.h"

LocalPatcherData::LocalPatcherData(const locations::Launcher& locations)
    : m_locations(locations)
{
}

bool LocalPatcherData::isInstalled() const
{
    qInfo("Checking whether patcher is installed.");

    QStringList mandatoryPatcherFiles;

    mandatoryPatcherFiles << m_locations.patcher().installationInfoFile();
    mandatoryPatcherFiles << m_locations.patcher().versionInfoFile();
    mandatoryPatcherFiles << m_locations.patcher().idInfoFile();

    for (int i = 0; i < mandatoryPatcherFiles.size(); i++)
    {
        if(!IOUtils::checkIfFileExists(mandatoryPatcherFiles[i]))
        {
            qInfo() << "Installation file doesn't exists - " << mandatoryPatcherFiles[i];
            return false;
        }
    }

    InstallationInfo installationInfo = readInstallationInfo();
    QDir installationDirectory(m_locations.patcher().directory());

    for (QString installedFile : installationInfo.installedFiles())
    {
        QString absolutePath = installationDirectory.absoluteFilePath(installedFile);
        if (!QDir(absolutePath).exists() && !QFile(absolutePath).exists())
        {
            qInfo() << "Installation entry doesn't exist - " << absolutePath;
            return false;
        }
    }

    return true;
}


bool LocalPatcherData::isInstalledSpecific(int t_version, const Secret& secret)
{
    if (isInstalled())
    {
        qInfo("Installation found. Checking version compatibility.");

        QString patcherId = IOUtils::readTextFromFile(m_locations.patcher().idInfoFile());

        int version = readVersion();

        qInfo() << "Local patcher id - " << patcherId;

        qInfo("Local version - %d", version);

        return patcherId == getPatcherId(secret) &&
               version == t_version;
    }

    qInfo("Installation not found.");
    return false;
}

InstallationInfo LocalPatcherData::install(const QString& t_downloadedPath, const Secret& patcherSecret, int t_version, CancellationToken cancellationToken)
{
    qInfo() << "Installing patcher (version "
            << t_version
            << ") from downloaded zip - "
            << t_downloadedPath;

    const auto& locations = m_locations;

    IOUtils::createDir(locations.patcher().directory());

    QStringList installationPatcherEntries;

    QFile downloadedFile(t_downloadedPath);

    return install(downloadedFile, patcherSecret, t_version, cancellationToken);
}

InstallationInfo LocalPatcherData::install(QIODevice& source, const Secret& patcherSecret, int version, CancellationToken cancellationToken)
{
    uninstall();
    const auto& locations = m_locations;
    InstallationInfo installationInfo;

    IOUtils::extractZip(source, locations.patcher().directory(), installationInfo, cancellationToken);

    IOUtils::writeTextToFile(locations.patcher().versionInfoFile(), QString::number(version));

#if defined(Q_OS_OSX) || defined(Q_OS_UNIX)
    for (QString entry : installationInfo.installedFiles())
    {
        auto effectivePath = QDir(locations.patcher().directory()).filePath(entry);
        system(QString("chmod +x \"%1\"").arg(effectivePath).toStdString().c_str());
    }
#endif

    QDir patcherDir(locations.patcher().directory());
    IOUtils::writeTextToFile(locations.patcher().launcherLocationFile(),
                patcherDir.relativeFilePath(locations.executable()));

    IOUtils::writeTextToFile(locations.patcher().idInfoFile(), getPatcherId(patcherSecret));

    QFile installationInfoFile(locations.patcher().installationInfoFile());
    installationInfoFile.open(QIODevice::WriteOnly);

    installationInfo.saveTo(installationInfoFile);

    return installationInfo;
}

void LocalPatcherData::start(const Secret& appSecret, std::shared_ptr<LockFile> lockFile, data::NetworkStatus networkStatus)
{
    qInfo("Starting patcher.");

    QString applicationSecret = QString::fromUtf8(appSecret.encoded().toBase64());

    PatcherManifestContext manifestContext;
    manifestContext.defineSymbol("{installdir}", m_locations.application().directory());
    manifestContext.defineSymbol("{exedir}", m_locations.patcher().directory());
    manifestContext.defineSymbol("{secret}", applicationSecret);
    manifestContext.defineSymbol("{lockfile}", QDir(Config::lockFileName).absolutePath());
    manifestContext.defineSymbol("{network-status}", data::ToString(networkStatus));

    auto manifest = readPatcherManifset();

    QString target = manifest.makeTarget(manifestContext);
    QStringList targetArguments = manifest.makeArguments(manifestContext);

    qDebug() << "Preparing execute target: " << target << " with arguments: " << targetArguments;

    if (lockFile)
    {
        qDebug("Releasing the lock file");
        lockFile->cede();
    }

    if (!QProcess::startDetached(target, targetArguments))
    {
        throw FatalException("Failed to start the patcher");
    }
}

void LocalPatcherData::uninstall()
{
    qInfo("Uninstalling patcher.");

    if (!QFile::exists(m_locations.patcher().installationInfoFile()))
    {
        qInfo("Missing installation info!");
    }
    else
    {
        QStringList installFiles =
                IOUtils::readTextFromFile(m_locations.patcher().installationInfoFile())
                .split(QChar('\n'));

        for (const auto& filePath : installFiles)
        {
            QFileInfo fileInfo(filePath);

            if (fileInfo.exists())
            {
                if (fileInfo.isFile())
                {
                    qInfo() << "Deleting file " << filePath;
                    QFile::remove(filePath);
                }
                else if (fileInfo.isDir())
                {
                    qInfo() << "Deleting directory " << filePath;
                    QDir(filePath).removeRecursively();
                }
            }
        }
    }
}

int LocalPatcherData::readVersion()
{
    qInfo("Reading version info of installed patcher.");

    QString versionInfoFileContents = IOUtils::readTextFromFile(m_locations.patcher().versionInfoFile());

    int version = parseVersionInfoToNumber(versionInfoFileContents);

    return version;
}

QString LocalPatcherData::getPatcherId(const Secret& secret)
{
    QString id = secret.value().remove(0, 2);
    id.chop(2);
    return id;
}

int LocalPatcherData::parseVersionInfoToNumber(const QString& t_versionInfoFileContents)
{
    qInfo() << "Parsing version info to number - " << t_versionInfoFileContents;

    bool intParseResult;

    int version = t_versionInfoFileContents.toInt(&intParseResult);

    if (!intParseResult)
    {
        throw std::runtime_error("Couldn't parse version info to number.");
    }

    return version;
}

InstallationInfo LocalPatcherData::readInstallationInfo() const
{
    QFile installationInfoFile(m_locations.patcher().installationInfoFile());
    installationInfoFile.open(QIODevice::ReadOnly);

    return InstallationInfo::from(installationInfoFile);
}

PatcherManifest LocalPatcherData::readPatcherManifset() const
{
    qInfo("Reading patcher manifest.");

    auto manifestLocation = m_locations.patcher().manifestFile();

    try
    {
        PatcherManifest manifest = Utilities::parsePatcherManifest(manifestLocation);
        return manifest;
    }
    catch(InvalidFormatException&)
    {
        qWarning("Invalid format exception detected while parsing the patcher manifest. Launcher will try to read the old format.");
    }

    try
    {
        PatcherManifest manifest = Utilities::parseOldPatcherManifest(manifestLocation);
        return manifest;
    }
    catch(InvalidFormatException&)
    {
        qCritical("Launcher couldn't parse the old patcher manifest format.");
        throw;
    }
}
