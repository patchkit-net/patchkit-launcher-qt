/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "data.h"

#include <QFile>
#include <QCryptographicHash>
#include <memory>

#include "logger.h"
#include "executableresources.h"
#include "config.h"

Data::Data()
{
}

bool Data::canLoadFromConfig()
{
    QByteArray hash = QCryptographicHash::hash(QByteArray::fromRawData(Config::inlineData, CONFIG_INLINE_DATA_PLACEHOLDER_LENGTH), QCryptographicHash::Md5);
    return hash.toHex() != QString(CONFIG_INLINE_DATA_PLACEHOLDER_HASH);
}

Data Data::loadFromConfig()
{
    if (!canLoadFromConfig())
    {
        throw std::runtime_error("Can't load data from config.");
    }

    unsigned char byteOne = (unsigned char) Config::inlineData[0];
    unsigned char byteTwo = (unsigned char) Config::inlineData[1];

    unsigned int dataSize = 0 | byteOne | (byteTwo << 8);

    const char* dataInConfig = Config::inlineData + 2;

    qDebug("Data size: %d", dataSize);

    QByteArray dataArray(dataInConfig, dataSize);
    QDataStream dataStream(dataArray);

    Data data = loadFromDataStream(dataStream);

    return data;
}

Data Data::loadFromFile(const QString& t_filePath)
{
    qInfo() << "Loading launcher data from file " << t_filePath;

    QFile file(t_filePath);

    if (!file.open(QFile::ReadOnly))
    {
        throw std::runtime_error("Couldn't open launcher data file from - " + t_filePath.toStdString());
    }

    QDataStream fileStream(&file);

    Data data = loadFromDataStream(fileStream);

    file.close();

    return data;
}

QString Data::patcherSecret() const
{
    if (overwritePatcherSecret.isEmpty())
    {
        return m_patcherSecret;
    }
    return overwritePatcherSecret;
}

QString Data::applicationSecret() const
{
    return m_applicationSecret;
}

QByteArray Data::encodedApplicationSecret() const
{
    return m_encodedApplicationSecret;
}

 #ifdef Q_OS_WIN

Data Data::loadFromResources(const QString& t_applicationFilePath, int t_resourceId, int t_resourceTypeId)
{
    qInfo() << "Loading launcher data from resource of id "
            << t_resourceId
            << " and type "
            << t_resourceTypeId
            << " from file "
            << t_applicationFilePath;

    std::shared_ptr<QByteArray> resourceData = ExecutableResources::extract(t_applicationFilePath, t_resourceId, t_resourceTypeId);

    QDataStream resourceStream(resourceData.get(), QIODevice::ReadOnly);

    return loadFromDataStream(resourceStream);
}

#endif

Data Data::loadFromDataStream(QDataStream& t_dataStream)
{
    t_dataStream.setByteOrder(QDataStream::LittleEndian);

    QByteArray encodedPatcherSecret = readStringBytes(t_dataStream);
    QByteArray encodedApplicationSecret = readStringBytes(t_dataStream);

    return Data(encodedPatcherSecret, encodedApplicationSecret);
}

Data::Data(const QByteArray& t_encodedPatcherSecret, const QByteArray& t_encodedApplicationSecret) :
    m_encodedApplicationSecret(t_encodedApplicationSecret)
{
    m_patcherSecret = decodeString(t_encodedPatcherSecret);
    m_applicationSecret = decodeString(m_encodedApplicationSecret);
}

QByteArray Data::readStringBytes(QDataStream& t_dataStream)
{
    qint32 len;

    if (t_dataStream.readRawData(reinterpret_cast<char*>(&len), 4) != 4)
    {
        throw std::runtime_error("Corrupted data file.");
    }

    QByteArray bytes(new char[len], len);

    if (t_dataStream.readRawData(bytes.data(), len) != len)
    {
        throw std::runtime_error("Corrupted data file.");
    }

    return bytes;
}

QString Data::decodeString(const QByteArray& t_encodedSecret)
{
    char temp[t_encodedSecret.size()];
    memcpy(temp, t_encodedSecret.data(), t_encodedSecret.size());

    for (int i = 0; i < t_encodedSecret.size(); i++)
    {
        char b = temp[i];
        bool lsb = (b & 1) > 0;
        b = b >> 1;
        b = b | (lsb ? 128 : 0);
        b = static_cast<char>(~b);
        temp[i] = b;
    }

    return QString::fromUtf16(reinterpret_cast<const ushort*>(temp), t_encodedSecret.size() / 2);
}
