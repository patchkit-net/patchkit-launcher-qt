/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "ioutils.h"

#include <QtDebug>
#include <QDir>
#include <QTextStream>
#include <quazip.h>
#include <quazipfile.h>
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

void IOUtils::extractZip(QIODevice& source, const QString& t_extractPath, InstallationInfo& installationInfo, CancellationToken cancellationToken)
{
    QuaZip zipFile(&source);

    if (!zipFile.open(QuaZip::mdUnzip))
    {
        throw std::runtime_error("Couldn't open zip file");
    }

    zipFile.goToFirstFile();

    do
    {
        cancellationToken.throwIfCancelled();

        QString zipEntryName = zipFile.getCurrentFileName();

        QString zipEntryPath = QDir::cleanPath(t_extractPath + "/" + zipEntryName);

        if (zipEntryName.endsWith('/') || zipEntryName.endsWith('\\'))
        {
            createDir(zipEntryPath);
        }
        else
        {
            QuaZipFile zipEntry(&zipFile);
            extractZipFileEntry(zipEntry, zipEntryPath, cancellationToken);
        }

        installationInfo.add(zipEntryName);
    }
    while (zipFile.goToNextFile());

    zipFile.close();
}

void IOUtils::copyIODeviceData(QIODevice& t_readDevice, QIODevice& t_writeDevice, CancellationToken cancellationToken)
{
    const qint64 bufferSize = 4096;
    char buffer[bufferSize];

    while (!t_readDevice.atEnd())
    {
        cancellationToken.throwIfCancelled();
        qint64 readSize = t_readDevice.read(buffer, bufferSize);
        if (readSize > 0)
        {
            qint64 ret = t_writeDevice.write(buffer, readSize);
            if (ret == -1) {
                qCritical() << "Copying IO device data failed: " << t_writeDevice.errorString();
                throw FatalException(t_writeDevice.errorString());
            }
        }
    }
}

void IOUtils::extractZipFileEntry(QuaZipFile& t_zipEntry, const QString& t_zipEntryPath, CancellationToken cancellationToken)
{
    if (!t_zipEntry.open(QIODevice::ReadOnly) || t_zipEntry.getZipError() != UNZ_OK)
    {
        auto msg = QString("Couldn't read zip entry %1").arg(t_zipEntry.getFileName());
        throw std::runtime_error(msg.toStdString());
    }

    QFileInfo zipEntryFileInfo(t_zipEntryPath);

    createDir(zipEntryFileInfo.absolutePath());

    QFile zipEntryFile(zipEntryFileInfo.absoluteFilePath());

    if (!zipEntryFile.open(QIODevice::WriteOnly))
    {
        auto msg = QString("Couldn't open file %1 for extracting.").arg(zipEntryFileInfo.absoluteFilePath());
        throw std::runtime_error(msg.toStdString());
    }

    copyIODeviceData(dynamic_cast<QIODevice&>(t_zipEntry), zipEntryFile, cancellationToken);

    zipEntryFile.close();
}
