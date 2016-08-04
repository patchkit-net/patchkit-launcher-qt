/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include <QFile>
#include <memory>

#include "launcherdata.h"
#include "launcherexception.h"
#include "launcherlog.h"

LauncherData LauncherData::loadFromFile(const QString& t_fileName)
{
    logInfo("Loading launcher data from file %1", .arg(t_fileName));

    QFile file(t_fileName);

    if (!file.open(QFile::ReadOnly))
    {
        throw LauncherException(QString("Couldn't open launcher data file from %1").arg(t_fileName));
    }

    QDataStream fileStream(&file);

    fileStream.setByteOrder(QDataStream::LittleEndian);

    QByteArray encodedPatcherSecret = readStringBytes(fileStream);
    QByteArray encodedApplicationSecret = readStringBytes(fileStream);

    file.close();

    return LauncherData(encodedPatcherSecret, encodedApplicationSecret);
}

LauncherData::LauncherData(const QByteArray& t_encodedPatcherSecret, const QByteArray& t_encodedApplicationSecret) :
    m_encodedPatcherSecret(t_encodedPatcherSecret),
    m_encodedApplicationSecret(t_encodedApplicationSecret)
{
    m_patcherSecret = decodeSecret(m_encodedPatcherSecret);
    m_applicationSecret = decodeSecret(m_encodedApplicationSecret);
}

QByteArray LauncherData::readStringBytes(QDataStream& t_fileStream)
{
    qint32 len;
    t_fileStream >> len;

    QByteArray bytes(new char[len], len);

    t_fileStream.readRawData(bytes.data(), len);

    return bytes;
}

QString LauncherData::decodeSecret(const QByteArray& t_encodedSecret)
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
