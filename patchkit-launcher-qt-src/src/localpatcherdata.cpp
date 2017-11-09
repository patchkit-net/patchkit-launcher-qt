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

#include "logger.h"
#include "locations.h"
#include "ioutils.h"

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

void LocalPatcherData::start(const Data& data)
{
    qInfo("Starting patcher.");

    QString exeFileName;
    QString exeArguments;

    readPatcherManifset(exeFileName, exeArguments);

    qDebug() << "Preparing run command from format - " << exeFileName << " " << exeArguments;

    exeFileName = formatPatcherManifestString(exeFileName, data.encodedApplicationSecret());
    exeArguments = formatPatcherManifestString(exeArguments, data.encodedApplicationSecret());

    qDebug() << "Starting process with command - " << exeFileName << " " << exeArguments;

    QProcess::startDetached(exeFileName + " " + exeArguments);
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

void LocalPatcherData::readPatcherManifset(QString& t_exeFileName, QString& t_exeArguments) const
{
    qInfo("Reading patcher manifest.");

    QJsonDocument jsonDocument = QJsonDocument::fromJson(IOUtils::readTextFromFile(Locations::getInstance().patcherManifestFilePath()).toUtf8());

    if (!jsonDocument.isObject())
    {
        throw std::runtime_error("Invaild format of patcher manifest file.");
    }

    QJsonObject jsonObject = jsonDocument.object();

    if (!jsonObject.contains("exe_fileName") || !jsonObject.contains("exe_arguments"))
    {
        throw std::runtime_error("Invaild format of patcher manifest file.");
    }

    QJsonValue exeFileNameJsonValue = jsonObject.value("exe_fileName");
    QJsonValue exeArgumentsJsonValue = jsonObject.value("exe_arguments");

    if (!exeFileNameJsonValue.isString() || !exeArgumentsJsonValue.isString())
    {
        throw std::runtime_error("Invaild format of patcher manifest file.");
    }

    t_exeFileName = exeFileNameJsonValue.toString();
    t_exeArguments = exeArgumentsJsonValue.toString();
}

QString LocalPatcherData::formatPatcherManifestString(const QString& t_stringToFormat, const QByteArray& t_encodedApplicationSecret) const
{
    QString result(t_stringToFormat);

    QString applicationSecret = QString::fromUtf8(t_encodedApplicationSecret.toBase64());

    result = result.replace("{installdir}", Locations::getInstance().applicationInstallationDirPath());
    result = result.replace("{exedir}", Locations::getInstance().patcherDirectoryPath());
    result = result.replace("{secret}", applicationSecret);

    return result;
}
