/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include <QFile>
#include <memory>

#include "launcherdata.h"
#include "launcherexception.h"
#include "launcherlog.h"
#include "launcherwindowsexeresources.h"

LauncherData::LauncherData()
{
}

LauncherData LauncherData::loadFromFile(const QString& t_filePath)
{
    logInfo("Loading launcher data from file %1", .arg(t_filePath));

    QFile file(t_filePath);

    if (!file.open(QFile::ReadOnly))
    {
        throw LauncherException(QString("Couldn't open launcher data file from %1").arg(t_filePath));
    }

    QDataStream fileStream(&file);

    LauncherData data = loadFromDataStream(fileStream);

    file.close();

    return data;
}

#ifdef Q_OS_WIN

LauncherData LauncherData::loadFromResource(const QString& t_applicationFilePath, int t_resourceId, int t_resourceTypeId)
{
    logInfo("Loading launcher data from resource of id %1 and type id %2 from file %3", .arg(QString::number(t_resourceId),
        QString::number(t_resourceTypeId),
        t_applicationFilePath));

    std::shared_ptr<QByteArray> resourceData = LauncherWindowsExeResources::extract(t_applicationFilePath, t_resourceId, t_resourceTypeId);

    QDataStream resourceStream(resourceData.get(), QIODevice::ReadOnly);

    return loadFromDataStream(resourceStream);
}

#endif

LauncherData LauncherData::loadFromDataStream(QDataStream& t_dataStream)
{
    t_dataStream.setByteOrder(QDataStream::LittleEndian);

    QByteArray encodedPatcherSecret = readStringBytes(t_dataStream);
    QByteArray encodedApplicationSecret = readStringBytes(t_dataStream);

    return LauncherData(encodedPatcherSecret, encodedApplicationSecret);
}

LauncherData::LauncherData(const QByteArray& t_encodedPatcherSecret, const QByteArray& t_encodedApplicationSecret) :
    m_encodedPatcherSecret(t_encodedPatcherSecret),
    m_encodedApplicationSecret(t_encodedApplicationSecret)
{
    m_patcherSecret = decodeString(m_encodedPatcherSecret);
    m_applicationSecret = decodeString(m_encodedApplicationSecret);
}

QByteArray LauncherData::readStringBytes(QDataStream& t_dataStream)
{
    qint32 len;

    if (t_dataStream.readRawData(reinterpret_cast<char*>(&len), 4) != 4)
    {
        throw LauncherException("Corrupted data file.");
    }

    QByteArray bytes(new char[len], len);

    if (t_dataStream.readRawData(bytes.data(), len) != len)
    {
        throw LauncherException("Corrupted data file.");
    }

    return bytes;
}

QString LauncherData::decodeString(const QByteArray& t_encodedSecret)
{
    std::unique_ptr<char> temp(new char[t_encodedSecret.size()]);
    memcpy(temp.get(), t_encodedSecret.data(), t_encodedSecret.size());

    for (int i = 0; i < t_encodedSecret.size(); i++)
    {
        char b = temp.get()[i];
        bool lsb = (b & 1) > 0;
        b = b >> 1;
        b = b | (lsb ? 128 : 0);
        b = static_cast<char>(~b);
        temp.get()[i] = b;
    }

    return QString::fromUtf16(reinterpret_cast<const ushort*>(temp.get()), t_encodedSecret.size() / 2);
}
