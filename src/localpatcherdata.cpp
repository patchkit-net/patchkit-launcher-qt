/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "localpatcherdata.h"

#include <memory>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QTextStream>

#include "logger.h"
#include "locations.h"
#include "exception.h"

bool LocalPatcherData::isInstalled()
{
    logInfo("Checking whether patcher is installed.");

    QStringList filesToCheck;

    filesToCheck << Locations::patcherInstallationInfoFilePath();
    filesToCheck << Locations::patcherVersionInfoFilePath();
    filesToCheck << Locations::patcherIdInfoFilePath();

    if (checkIfFilesExist(filesToCheck))
    {
        QStringList installFiles = readFileContents(Locations::patcherInstallationInfoFilePath()).split(QChar('\n'));

        return checkIfFilesExist(installFiles);
    }

    return false;
}


bool LocalPatcherData::isInstalledSpecific(int t_version, const Data& t_data)
{
    if(isInstalled())
    {
        return readFileContents(Locations::patcherIdInfoFilePath()) == getPatcherId(t_data) &&
            readVersion() == t_version;
    }
    return false;
}

void LocalPatcherData::install(const QString& t_downloadedPath, const Data& t_data, int t_version)
{
    uninstall();

    logInfo("Installing patcher (version %1) from downloaded zip - %2", .arg(QString::number(t_version), t_downloadedPath));

    createDirIfNotExists(Locations::patcherDirectoryPath());

    QStringList installationInfoFileList;

    extractZip(t_downloadedPath, Locations::patcherDirectoryPath(), installationInfoFileList);

    installationInfoFileList.append(Locations::patcherInstallationInfoFilePath());
    installationInfoFileList.append(Locations::patcherVersionInfoFilePath());
    installationInfoFileList.append(Locations::patcherIdInfoFilePath());

    writeFileContents(Locations::patcherVersionInfoFilePath(), QString::number(t_version));

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

    writeFileContents(Locations::patcherInstallationInfoFilePath(), installationInfoFileContents);
    writeFileContents(Locations::patcherIdInfoFilePath(), getPatcherId(t_data));

    QFile::remove(t_downloadedPath);
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

    if (!QFile::exists(Locations::patcherInstallationInfoFilePath()))
    {
        logInfo("Missing installation info!");
    }
    else
    {
        QStringList installFiles = readFileContents(Locations::patcherInstallationInfoFilePath()).split(QChar('\n'));

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

void LocalPatcherData::writeFileContents(const QString& t_filePath, const QString& t_fileContents)
{
    logInfo("Writing file contents to %1", .arg(t_filePath));

    QFile file(t_filePath);

    if (!file.open(QFile::WriteOnly))
    {
        throw Exception(QString("Couldn't open file %1 for writing").arg(t_filePath));
    }

    QTextStream fileTextStream(&file);
    fileTextStream << t_fileContents;
    file.close();
}

QString LocalPatcherData::readFileContents(const QString& t_filePath)
{
    logInfo("Reading file contents from %1", .arg(t_filePath));

    QFile file(t_filePath);

    if (!file.open(QFile::ReadOnly))
    {
        throw Exception(QString("Couldn't open file %1 for reading").arg(t_filePath));
    }

    QString fileContents(file.readAll());

    file.close();

    return fileContents;
}

bool LocalPatcherData::checkIfFilesExist(const QStringList& t_filesList)
{
    logInfo("Checking whether files from list exists.");

    for (int i = 0; i < t_filesList.size(); i++)
    {
        if (!QFile::exists(t_filesList[i]))
        {
            logInfo("%1 doesn't exists.", .arg(t_filesList[i]));
            return false;
        }

        logInfo("%1 exists.", .arg(t_filesList[i]));
    }

    return true;
}

void LocalPatcherData::createDirIfNotExists(const QString& t_dirPath)
{
    logInfo("Creating directory - %1", .arg(t_dirPath));

    QDir dir = QDir(t_dirPath);

    if (!dir.exists())
    {
        if (!dir.mkpath("."))
        {
            throw Exception(QString("Couldn't create directory - %1").arg(t_dirPath));
        }
    }
}

void LocalPatcherData::extractZip(const QString& t_zipFilePath, const QString& t_extractDirPath, QStringList& t_extractedFilesList)
{
    logInfo("Extracting zip file - %1", .arg(t_zipFilePath));

    QuaZip zipFile(t_zipFilePath);

    if (!zipFile.open(QuaZip::mdUnzip))
    {
        throw Exception("Couldn't open zip file.");
    }

    zipFile.goToFirstFile();

    do
    {
        QString zipEntryName = zipFile.getCurrentFileName();

        QString zipEntryPath = QDir::cleanPath(t_extractDirPath + "/" + zipEntryName);

        if (isDirZipEntry(zipEntryName))
        {
            extractDirZipEntry(zipEntryPath);
        }
        else
        {
            QuaZipFile zipEntry(&zipFile);
            extractFileZipEntry(zipEntry, zipEntryPath);
        }

        t_extractedFilesList.append(zipEntryPath);
    }
    while (zipFile.goToNextFile());

    zipFile.close();
}

void LocalPatcherData::extractDirZipEntry(const QString& t_zipEntryPath)
{
    createDirIfNotExists(t_zipEntryPath);
}

void LocalPatcherData::extractFileZipEntry(QuaZipFile& t_zipEntry, const QString& t_zipEntryPath)
{
    if (!t_zipEntry.open(QIODevice::ReadOnly) || t_zipEntry.getZipError() != UNZ_OK)
    {
        throw Exception("Couldn't read zip entry.");
    }

    QFileInfo zipEntryFileInfo(t_zipEntryPath);

    createDirIfNotExists(zipEntryFileInfo.absolutePath());

    QFile zipEntryFile(zipEntryFileInfo.absoluteFilePath());

    if (!zipEntryFile.open(QIODevice::WriteOnly))
    {
        throw Exception("Couldn't open file for extracting.");
    }

    copyDeviceData(reinterpret_cast<QIODevice&>(t_zipEntry), zipEntryFile);

    zipEntryFile.close();
}

bool LocalPatcherData::isDirZipEntry(const QString& t_zipEntryName)
{
    return t_zipEntryName.endsWith('/') || t_zipEntryName.endsWith('\\');
}

void LocalPatcherData::copyDeviceData(QIODevice& readDevice, QIODevice& writeDevice)
{
    qint64 bufferSize = 4096;
    std::unique_ptr<char> buffer(new char[bufferSize]);

    while (!readDevice.atEnd())
    {
        qint64 readSize = readDevice.read(buffer.get(), bufferSize);
        if (readSize > 0)
        {
            writeDevice.write(buffer.get(), readSize);
        }
    }
}

int LocalPatcherData::readVersion()
{
    logInfo("Reading version info of installed patcher.");

    QString versionInfoFileContents = readFileContents(Locations::patcherVersionInfoFilePath());

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
        throw Exception("Couldn't parse version info to number.");
    }

    return version;
}

void LocalPatcherData::readPatcherManifset(QString& t_exeFileName, QString& t_exeArguments) const
{
    logInfo("Reading patcher manifest.");

    QJsonDocument jsonDocument = QJsonDocument::fromJson(readFileContents(Locations::patcherManifestFilePath()).toUtf8());

    if (!jsonDocument.isObject())
    {
        throw Exception("Invaild format of patcher manifest file.");
    }

    QJsonObject jsonObject = jsonDocument.object();

    if (!jsonObject.contains("exe_fileName") || !jsonObject.contains("exe_arguments"))
    {
        throw Exception("Invaild format of patcher manifest file.");
    }

    QJsonValue exeFileNameJsonValue = jsonObject.value("exe_fileName");
    QJsonValue exeArgumentsJsonValue = jsonObject.value("exe_arguments");

    if (!exeFileNameJsonValue.isString() || !exeArgumentsJsonValue.isString())
    {
        throw Exception("Invaild format of patcher manifest file.");
    }

    t_exeFileName = exeFileNameJsonValue.toString();
    t_exeArguments = exeArgumentsJsonValue.toString();
}

QString LocalPatcherData::formatPatcherManifestString(const QString& t_stringToFormat, const QByteArray& t_encodedApplicationSecret) const
{
    QString result(t_stringToFormat);

    QString applicationSecret = QString::fromUtf8(t_encodedApplicationSecret.toBase64());

    result = result.replace("{installdir}", Locations::applicationInstallationDirPath());
    result = result.replace("{exedir}", Locations::patcherDirectoryPath());
    result = result.replace("{secret}", applicationSecret);

    return result;
}
