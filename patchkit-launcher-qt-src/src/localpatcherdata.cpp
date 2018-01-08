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

#include "config.h"
#include "logger.h"
#include "locations.h"
#include "ioutils.h"
#include "lockfile.h"
#include "patchermanifest.h"
#include "customexceptions.h"
#include "utilities.h"

bool LocalPatcherData::isInstalled() const
{
    qInfo("Checking whether patcher is installed.");

    QStringList mandatoryPatcherFiles;

    mandatoryPatcherFiles << Locations::getInstance().patcherInstallationInfoFilePath();
    mandatoryPatcherFiles << Locations::getInstance().patcherVersionInfoFilePath();
    mandatoryPatcherFiles << Locations::getInstance().patcherIdInfoFilePath();

    for (int i = 0; i < mandatoryPatcherFiles.size(); i++)
    {
        if(!IOUtils::checkIfFileExists(mandatoryPatcherFiles[i]))
        {
            qInfo() << "Installation file doesn't exists - " << mandatoryPatcherFiles[i];
            return false;
        }
    }

    QStringList installationPatcherEntries = IOUtils::readTextFromFile(Locations::getInstance().patcherInstallationInfoFilePath()).split(QChar('\n'));

    for (int i = 0; i < installationPatcherEntries.size(); i++)
    {
        QString entryPath = QDir::cleanPath(Locations::getInstance().patcherDirectoryPath() + "/" + installationPatcherEntries[i]);

        if(installationPatcherEntries[i].endsWith("/") || installationPatcherEntries[i].endsWith("\\"))
        {
            if(!IOUtils::checkIfDirExists(entryPath))
            {
                qInfo() << "Installation directory doesn't exists - " << entryPath;
                return false;
            }
        }
        else
        {
            if(!IOUtils::checkIfFileExists(entryPath))
            {
                qInfo() << "Installation file doesn't exists - " << entryPath;
                return false;
            }
        }
    }

    return true;
}


bool LocalPatcherData::isInstalledSpecific(int t_version, const Data& t_data)
{
    if (isInstalled())
    {
        qInfo("Installation found. Checking version compatibility.");

        QString patcherId = IOUtils::readTextFromFile(Locations::getInstance().patcherIdInfoFilePath());

        int version = readVersion();

        qInfo() << "Local patcher id - " << patcherId;

        qInfo("Local version - %d", version);

        return patcherId == getPatcherId(t_data) &&
               version == t_version;
    }

    qInfo("Installation not found.");

    return false;
}

void LocalPatcherData::install(const QString& t_downloadedPath, const Data& t_data, int t_version)
{
    uninstall();

    qInfo() << "Installing patcher (version "
            << t_version
            << ") from downloaded zip - "
            << t_downloadedPath;

    const auto& locations = Locations::getInstance();

    IOUtils::createDir(locations.patcherDirectoryPath());

    QStringList installationPatcherEntries;

    IOUtils::extractZip(t_downloadedPath, locations.patcherDirectoryPath(), installationPatcherEntries);

    IOUtils::writeTextToFile(locations.patcherVersionInfoFilePath(), QString::number(t_version));

    QString installationInfoFileContents = "";

    for (int i = 0; i < installationPatcherEntries.size(); i++)
    {
        QString entryPath = QDir::cleanPath(locations.patcherDirectoryPath() + "/" + installationPatcherEntries[i]);

#if defined(Q_OS_OSX) || defined(Q_OS_UNIX)
        system(QString("chmod +x \"%1\"").arg(entryPath).toStdString().c_str());
#endif

        installationInfoFileContents += installationPatcherEntries[i];

        if (i != installationPatcherEntries.size() - 1)
        {
            installationInfoFileContents += "\n";
        }
    }

    QDir patcherDir(locations.patcherDirectoryPath());
    IOUtils::writeTextToFile(locations.patcherLauncherPathFilePath(),
                patcherDir.relativeFilePath(locations.applicationFilePath()));

    IOUtils::writeTextToFile(locations.patcherInstallationInfoFilePath(), installationInfoFileContents);
    IOUtils::writeTextToFile(locations.patcherIdInfoFilePath(), getPatcherId(t_data));
}

void LocalPatcherData::start(const Data& t_data)
{
    qInfo("Starting patcher.");

    QString applicationSecret = QString::fromUtf8(t_data.encodedApplicationSecret().toBase64());

    PatcherManifestContext manifestContext;
    manifestContext.defineSymbol("{installdir}", Locations::getInstance().applicationInstallationDirPath());
    manifestContext.defineSymbol("{exedir}", Locations::getInstance().patcherDirectoryPath());
    manifestContext.defineSymbol("{secret}", applicationSecret);
    manifestContext.defineSymbol("{lockfile}", QDir(Config::lockFileName).absolutePath());

    auto manifest = readPatcherManifset();

    QString target = manifest.makeTarget(manifestContext);
    QStringList targetArguments = manifest.makeArguments(manifestContext);

    qDebug() << "Preparing execute target: " << target << " with arguments: " << targetArguments;

    if (!QProcess::startDetached(target, targetArguments))
    {
        throw FatalException("Failed to start the patcher.");
    }
}

void LocalPatcherData::uninstall()
{
    qInfo("Uninstalling patcher.");

    if (!QFile::exists(Locations::getInstance().patcherInstallationInfoFilePath()))
    {
        qInfo("Missing installation info!");
    }
    else
    {
        QStringList installFiles = IOUtils::readTextFromFile(Locations::getInstance().patcherInstallationInfoFilePath()).split(QChar('\n'));

        for (int i = 0; i < installFiles.size(); i++)
        {
            QFileInfo fileInfo(installFiles[i]);

            if (fileInfo.exists())
            {
                if (fileInfo.isFile())
                {
                    qInfo() << "Deleting file " << installFiles[i];
                    QFile::remove(installFiles[i]);
                }
                else if (fileInfo.isDir())
                {
                    qInfo() << "Deleting directory " << installFiles[i];
                    QDir(installFiles[i]).removeRecursively();
                }
            }
        }
    }
}

int LocalPatcherData::readVersion()
{
    qInfo("Reading version info of installed patcher.");

    QString versionInfoFileContents = IOUtils::readTextFromFile(Locations::getInstance().patcherVersionInfoFilePath());

    int version = parseVersionInfoToNumber(versionInfoFileContents);

    return version;
}

QString LocalPatcherData::getPatcherId(const Data& t_data)
{
    QString id = t_data.patcherSecret().remove(0, 2);
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

PatcherManifest LocalPatcherData::readPatcherManifset() const
{
    qInfo("Reading patcher manifest.");
    return Utilities::parsePatcherManifest(Locations::getInstance().patcherManifestFilePath());
}
