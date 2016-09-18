/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "localpatcherdata.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QTextStream>

#include "logger.h"
#include "locations.h"
#include "ioutils.h"

bool LocalPatcherData::isInstalled()
{
    logInfo("Checking whether patcher is installed.");

    QStringList filesToCheck;

    filesToCheck << Locations::getInstance().patcherInstallationInfoFilePath();
    filesToCheck << Locations::getInstance().patcherVersionInfoFilePath();
    filesToCheck << Locations::getInstance().patcherIdInfoFilePath();

    if (IOUtils::checkIfAllFilesExist(filesToCheck))
    {
        QStringList installFiles = IOUtils::readTextFromFile(Locations::getInstance().patcherInstallationInfoFilePath()).split(QChar('\n'));

        return IOUtils::checkIfAllFilesExist(installFiles);
    }

    return false;
}


bool LocalPatcherData::isInstalledSpecific(int t_version, const Data& t_data)
{
    if(isInstalled())
    {
        return IOUtils::readTextFromFile(Locations::getInstance().patcherIdInfoFilePath()) == getPatcherId(t_data) &&
            readVersion() == t_version;
    }
    return false;
}

void LocalPatcherData::install(const QString& t_downloadedPath, const Data& t_data, int t_version)
{
    uninstall();

    logInfo("Installing patcher (version %1) from downloaded zip - %2", .arg(QString::number(t_version), t_downloadedPath));

    IOUtils::createDir(Locations::getInstance().patcherDirectoryPath());

    QStringList installationInfoFileList;

    IOUtils::extractZip(t_downloadedPath, Locations::getInstance().patcherDirectoryPath(), installationInfoFileList);

    installationInfoFileList.append(Locations::getInstance().patcherInstallationInfoFilePath());
    installationInfoFileList.append(Locations::getInstance().patcherVersionInfoFilePath());
    installationInfoFileList.append(Locations::getInstance().patcherIdInfoFilePath());

    IOUtils::writeTextToFile(Locations::getInstance().patcherVersionInfoFilePath(), QString::number(t_version));

    QString installationInfoFileContents = "";

    for (int i = 0; i < installationInfoFileList.size(); i++)
    {
        QFileInfo installationFileInfo(installationInfoFileList[i]);

#if defined(Q_OS_OSX) || defined(Q_OS_UNIX)

        system(QString("chmod +x \"%1\"").arg(installationFileInfo.absoluteFilePath()).toStdString().c_str());

#endif

        installationInfoFileContents += installationInfoFileList[i];

        if (i != installationInfoFileList.size() - 1)
        {
            installationInfoFileContents += "\n";
        }
    }

    IOUtils::writeTextToFile(Locations::getInstance().patcherInstallationInfoFilePath(), installationInfoFileContents);
    IOUtils::writeTextToFile(Locations::getInstance().patcherIdInfoFilePath(), getPatcherId(t_data));
}

void LocalPatcherData::start(const Data& data)
{
    logInfo("Starting patcher.");

    QString exeFileName;
    QString exeArguments;

    readPatcherManifset(exeFileName, exeArguments);

    logDebug("Preparing run command from format - %1 %2", .arg(exeFileName, exeArguments));

    exeFileName = formatPatcherManifestString(exeFileName, data.encodedApplicationSecret());
    exeArguments = formatPatcherManifestString(exeArguments, data.encodedApplicationSecret());

    logDebug("Starting process with command - %1 %2", .arg(exeFileName, exeArguments));

    QProcess::startDetached(exeFileName + " " + exeArguments);
}

void LocalPatcherData::uninstall()
{
    logInfo("Uninstalling patcher.");

    if (!QFile::exists(Locations::getInstance().patcherInstallationInfoFilePath()))
    {
        logInfo("Missing installation info!");
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
                    logInfo("Deleting file %1", .arg(installFiles[i]));
                    QFile::remove(installFiles[i]);
                }
                else if (fileInfo.isDir())
                {
                    logInfo("Deleting directory %1", .arg(installFiles[i]));
                    QDir(installFiles[i]).removeRecursively();
                }
            }
        }
    }
}

int LocalPatcherData::readVersion()
{
    logInfo("Reading version info of installed patcher.");

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
    logInfo("Parsing version info to number - %1", .arg(t_versionInfoFileContents));

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
    logInfo("Reading patcher manifest.");

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
