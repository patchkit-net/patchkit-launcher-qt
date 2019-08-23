/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "data.h"

#include <QFile>
#include <QCryptographicHash>
#include <QtDebug>
#include <QProcessEnvironment>
#include <memory>

#include "executableresources.h"
#include "config.h"
#include "data/secretencoding.h"

Data Data::overridePatcherSecret(Data&& data, Secret patcherSecret)
{
    return Data(patcherSecret, data.applicationSecret());
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

Secret Data::patcherSecret() const
{
    QProcessEnvironment processEnv = QProcessEnvironment::systemEnvironment();
    if (processEnv.contains(Config::patcherSecretOverrideEnvVar))
    {
        return Secret::from(processEnv.value(Config::patcherSecretOverrideEnvVar, ""));
    }
    return m_patcherSecret;
}

Secret Data::applicationSecret() const
{
    return m_applicationSecret;
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

    return Data::fromEncoded(encodedPatcherSecret, encodedApplicationSecret);
}

Data::Data(const QString& patcherSecret, const QString& appSecret)
    : m_patcherSecret(patcherSecret)
    , m_applicationSecret(appSecret)
{
}

Data Data::fromEncoded(const QByteArray& t_encodedPatcherSecret, const QByteArray& t_encodedApplicationSecret)
{
    return Data(
                data::secret::decode(t_encodedPatcherSecret),
                data::secret::decode(t_encodedApplicationSecret));
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

Secret::operator const QString&() const
{
    return m_value;
}

Secret Secret::fromEncoded(const QByteArray& encodedSecert)
{
    QString decoded = data::secret::decode(encodedSecert);

    return Secret(decoded);
}

Secret Secret::from(const QString& secret)
{
    return Secret(secret);
}

QString Secret::value() const
{
    return m_value;
}

QByteArray Secret::encoded() const
{
    return data::secret::encode(m_value);
}

Secret::Secret(const QString& secret)
    : m_value(secret)
{
}
