/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#ifndef PATCHKITLOCALPATCHER_H
#define PATCHKITLOCALPATCHER_H

#include "localpatcher.h"
#include "quazipfile.h"

class PatchKitLocalPatcher : public LocalPatcher
{
public:
    bool isInstalled() override;

    int getVersion() override;

    void install(const QString& t_downloadedPath, int t_version) override;

    void uninstall() override;

    void start(const LauncherData& t_data) override;

    void cancel() override;

private:
    static void writeFileContents(const QString& t_filePath, const QString& t_fileContents);

    static QString readFileContents(const QString& t_filePath);

    static int parseVersionInfoToNumber(const QString& t_versionInfoFileContents);

    static bool checkIfFilesExist(const QStringList& t_filesList);

    static void createDirIfNotExists(const QString& t_dirPath);

    static void extractZip(const QString& t_zipFilePath,
                           const QString& t_extractDirPath,
                           QStringList& t_extractedEntriesList);

    static void extractDirZipEntry(const QString& t_zipEntryPath);

    static void extractFileZipEntry(QuaZipFile& t_zipEntry, const QString& t_zipEntryPath);

    static bool isDirZipEntry(const QString& t_zipEntryName);

    static void copyDeviceData(QIODevice& t_readDevice, QIODevice& t_writeDevice);

    void readPatcherManifset(QString& t_exeFileName,
                             QString& t_exeArguments) const;

    QString formatPatcherManifest(const QString& t_stringToFormat,
                                  const QByteArray& t_encodedApplicationSecret) const;

    const QString patcherDirectoryPath = "patcher";
    const QString installationInfoFilePath = patcherDirectoryPath + "/install_info";
    const QString versionInfoFilePath = patcherDirectoryPath + "/version_info";
    const QString manifestFilePath = patcherDirectoryPath + "/patcher.manifest";

    // Note that this path is relative to patcherDirectoryPath.
    const QString installationDirectoryPath = "../app";
};

#endif // PATCHKITLOCALPATCHER_H
