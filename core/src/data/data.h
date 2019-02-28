/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QDataStream>

struct Data
{
    Data();

    static Data overwritePatcherSecret(const Data& original, const QString& patcherSecret);

    static bool canLoadFromConfig();

    static Data loadFromConfig();

    static Data loadFromFile(const QString& t_filePath);

#ifdef Q_OS_WIN
    static Data loadFromResources(const QString& t_applicationFilePath, int t_resourceId, int t_resourceTypeId);
#endif

    QString patcherSecret() const;

    QString applicationSecret() const;

    QByteArray encodedApplicationSecret() const;

private:
    static Data fromEncoded(const QByteArray& t_encodedPatcherSecret, const QByteArray& t_encodedApplicationSecret);

    static Data loadFromDataStream(QDataStream& t_dataStream);

    Data(const QString& patcherSecret, const QString& appSecret);

    QString m_patcherSecret;
    QString m_applicationSecret;

    static QByteArray readStringBytes(QDataStream& t_dataStream);
};
