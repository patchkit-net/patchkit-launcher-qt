/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <quazip/quazipfile.h>

class IOUtils
{
public:
    static void createDir(const QString& t_dirPath);

    static QString readTextFromFile(const QString& t_filePath);

    static void writeTextToFile(const QString& t_filePath, const QString& t_fileContents);

    static void writeDataToFile(const QString& t_filePath, const QByteArray& t_fileData);

    static bool checkIfDirExists(const QString& t_dirPath);

    static bool checkIfFileExists(const QString& t_filePath);

    static void extractZip(const QString& t_zipPath, const QString& t_extractPath, QStringList& t_extractedEntries);

    static void copyIODeviceData(QIODevice& t_readDevice, QIODevice& t_writeDevice);

private:
    static void extractZipFileEntry(QuaZipFile& t_zipEntry, const QString& t_zipEntryPath);
};
