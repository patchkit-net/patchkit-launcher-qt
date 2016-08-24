/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QDataStream>

struct Data
{
    Data();

    static Data loadFromFile(const QString& t_filePath);

#ifdef Q_OS_WIN
    static Data loadFromResources(const QString& t_applicationFilePath, int t_resourceId, int t_resourceTypeId);
#endif

    QString overwritePatcherSecret;

    QString patcherSecret() const
    {
        if (overwritePatcherSecret.isEmpty())
        {
            return m_patcherSecret;
        }
        return overwritePatcherSecret;
    }

    QString applicationSecret() const
    {
        return m_applicationSecret;
    }

    QByteArray encodedApplicationSecret() const
    {
        return m_encodedApplicationSecret;
    }

private:
    static Data loadFromDataStream(QDataStream& t_dataStream);

    QString m_patcherSecret;
    QString m_applicationSecret;
    QByteArray m_encodedApplicationSecret;

    Data(const QByteArray& t_encodedPatcherSecret, const QByteArray& t_encodedApplicationSecret);

    static QByteArray readStringBytes(QDataStream& t_dataStream);
    static QString decodeString(const QByteArray& t_encodedSecret);
};
