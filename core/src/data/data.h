/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QDataStream>

struct Secret
{
public:
    Secret(const QString& secret);

    static Secret fromEncoded(const QByteArray& encodedSecert);
    static Secret from(const QString& secret);

    QString value() const;
    QByteArray encoded() const;

    operator const QString&() const;

private:
    QString m_value;
};

struct Data
{
    Data();

    static Data overridePatcherSecret(Data&& data, Secret patcherSecret);

    static bool canLoadFromConfig();

    static Data loadFromConfig();

    static Data loadFromFile(const QString& t_filePath);

#ifdef Q_OS_WIN
    static Data loadFromResources(const QString& t_applicationFilePath, int t_resourceId, int t_resourceTypeId);
#endif

    Secret patcherSecret() const;

    Secret applicationSecret() const;

private:
    static Data fromEncoded(const QByteArray& t_encodedPatcherSecret, const QByteArray& t_encodedApplicationSecret);

    static Data loadFromDataStream(QDataStream& t_dataStream);

    Data(const QString& patcherSecret, const QString& appSecret);

    Secret m_patcherSecret;
    Secret m_applicationSecret;

    static QByteArray readStringBytes(QDataStream& t_dataStream);
};
