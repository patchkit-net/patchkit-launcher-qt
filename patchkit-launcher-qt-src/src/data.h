/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QDataStream>

struct Data
{
    Data();
    Data(const Data& t_other);
    Data(const QString& t_patcherSecret, const QByteArray& t_encodedApplicationSecret);

    static bool canLoadFromConfig();

    static Data loadFromConfig();

    static Data loadFromFile(const QString& t_filePath);

#ifdef Q_OS_WIN
    static Data loadFromResources(const QString& t_applicationFilePath, int t_resourceId, int t_resourceTypeId);
#endif

    virtual QString patcherSecret() const;

    virtual QString applicationSecret() const;

    virtual QByteArray encodedApplicationSecret() const;

    static QString decodeEncodedApplicationSecret(const QByteArray& t_data);

private:
    static Data loadFromDataStream(QDataStream& t_dataStream);

    QByteArray m_encodedApplicationSecret;
    QString m_patcherSecret;

    static QByteArray readStringBytes(QDataStream& t_dataStream);
    static QString decodeString(const QByteArray& t_encodedSecret);
};
