/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>
#include <QString>

#include "launcherdata.h"
#include "quazipfile.h"

class LocalPatcher : public QObject
{
    Q_OBJECT

public:
    bool isInstalled();

    // Can assume that patcher is installed.
    int getVersion();

    // Can assume that patcher is uninstalled.
    void install(const QString& t_downloadedPath, int t_version);

    // Cannot assume that patcher is installed.
    void uninstall();

    // Can assume that patcher is installed.
    void start(const LauncherData& t_data);

public slots:
    // Cancels any above operation. If cancellation is successful, LauncherCancelledException is thrown by the operation.
    void cancel();

private:
    // Helper functions

    static void writeFileContents(const QString& t_filePath, const QString& t_fileContents);

    static QString readFileContents(const QString& t_filePath);

    static bool checkIfFilesExist(const QStringList& t_filesList);

    static void createDirIfNotExists(const QString& t_dirPath);

    static void extractZip(const QString& t_zipFilePath,
                           const QString& t_extractDirPath,
                           QStringList& t_extractedEntriesList);

    static void extractDirZipEntry(const QString& t_zipEntryPath);

    static void extractFileZipEntry(QuaZipFile& t_zipEntry, const QString& t_zipEntryPath);

    static bool isDirZipEntry(const QString& t_zipEntryName);

    static void copyDeviceData(QIODevice& t_readDevice, QIODevice& t_writeDevice);

    // Operations helper functions

    static int parseVersionInfoToNumber(const QString& t_versionInfoFileContents);

    void readPatcherManifset(QString& t_exeFileName,
                             QString& t_exeArguments) const;

    QString formatPatcherManifestString(const QString& t_stringToFormat,
                                        const QByteArray& t_encodedApplicationSecret) const;
};
