/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include "data/installationinfo.h"
#include "cancellation/cancellationtoken.h"

#include <QIODevice>
#include <quazipfile.h>

class IOUtils
{
public:
    static void createDir(const QString& t_dirPath);

    static QString readTextFromFile(const QString& t_filePath);

    static void writeTextToFile(const QString& t_filePath, const QString& t_fileContents);

    static void writeDataToFile(const QString& t_filePath, const QByteArray& t_fileData);

    static bool checkIfDirExists(const QString& t_dirPath);

    static bool checkIfFileExists(const QString& t_filePath);

    static void extractZip(QIODevice& source, const QString& t_extractPath, InstallationInfo& installationInfo, CancellationToken cancellationToken);

    static void copyIODeviceData(QIODevice& t_readDevice, QIODevice& t_writeDevice, CancellationToken cancellationToken);

private:
    static void extractZipFileEntry(QuaZipFile& t_zipEntry, const QString& t_zipEntryPath, CancellationToken cancellationToken);
};
