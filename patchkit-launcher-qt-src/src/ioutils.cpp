/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "ioutils.h"

#include <QDir>
#include <QTextStream>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#include <memory>

void IOUtils::createDir(const QString& t_dirPath)
{
    QDir dir(t_dirPath);
    dir.mkpath(".");
}

QString IOUtils::readTextFromFile(const QString& t_filePath)
{
    QFile file(t_filePath);

    if (!file.open(QFile::ReadOnly))
    {
        throw std::runtime_error("Couldn't open file for reading - " + t_filePath.toStdString());
    }

    return file.readAll();
}

void IOUtils::writeTextToFile(const QString& t_filePath, const QString& t_fileContents)
{
    QFile file(t_filePath);

    if (!file.open(QFile::WriteOnly))
    {
        throw std::runtime_error("Couldn't open file for writing - " + t_filePath.toStdString());
    }

    QTextStream fileTextStream(&file);
    fileTextStream << t_fileContents;
}

void IOUtils::writeDataToFile(const QString& t_filePath, const QByteArray& t_fileData)
{
    QFile file(t_filePath);

    if (!file.open(QFile::WriteOnly))
    {
        throw std::runtime_error("Couldn't open file for writing - " + t_filePath.toStdString());
    }

    file.write(t_fileData);
}

bool IOUtils::checkIfDirExists(const QString& t_dirPath)
{
    QFileInfo info(t_dirPath);

    return info.isDir();
}

bool IOUtils::checkIfFileExists(const QString& t_filePath)
{
    QFileInfo info(t_filePath);

    return info.isFile();
}

void IOUtils::extractZip(const QString& t_zipPath, const QString& t_extractPath, QStringList& t_extractedEntries)
{
    QuaZip zipFile(t_zipPath);

    if (!zipFile.open(QuaZip::mdUnzip))
    {
        throw std::runtime_error("Couldn't open zip file - " + t_zipPath.toStdString());
    }

    zipFile.goToFirstFile();

    do
    {
        QString zipEntryName = zipFile.getCurrentFileName();

        QString zipEntryPath = QDir::cleanPath(t_extractPath + "/" + zipEntryName);

        if (zipEntryName.endsWith('/') || zipEntryName.endsWith('\\'))
        {
            createDir(zipEntryPath);
        }
        else
        {
            QuaZipFile zipEntry(&zipFile);
            extractZipFileEntry(zipEntry, zipEntryPath);
        }

        t_extractedEntries.append(zipEntryName);
    }
    while (zipFile.goToNextFile());

    zipFile.close();
}

void IOUtils::copyIODeviceData(QIODevice& t_readDevice, QIODevice& t_writeDevice)
{
    const qint64 bufferSize = 4096;
    char buffer[bufferSize];

    while (!t_readDevice.atEnd())
    {
        qint64 readSize = t_readDevice.read(buffer, bufferSize);
        if (readSize > 0)
        {
            t_writeDevice.write(buffer, readSize);
        }
    }
}

void IOUtils::extractZipFileEntry(QuaZipFile& t_zipEntry, const QString& t_zipEntryPath)
{
    if (!t_zipEntry.open(QIODevice::ReadOnly) || t_zipEntry.getZipError() != UNZ_OK)
    {
        throw std::runtime_error("Couldn't read zip entry.");
    }

    QFileInfo zipEntryFileInfo(t_zipEntryPath);

    createDir(zipEntryFileInfo.absolutePath());

    QFile zipEntryFile(zipEntryFileInfo.absoluteFilePath());

    if (!zipEntryFile.open(QIODevice::WriteOnly))
    {
        throw std::runtime_error("Couldn't open file for extracting.");
    }

    copyIODeviceData(reinterpret_cast<QIODevice&>(t_zipEntry), zipEntryFile);

    zipEntryFile.close();
}
