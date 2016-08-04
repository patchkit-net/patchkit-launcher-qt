/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#ifndef LAUNCHERDATA_H
#define LAUNCHERDATA_H

#include <QDataStream>

struct LauncherData
{
    static LauncherData loadFromFile(const QString& t_fileName);

    QString patcherSecret() const
    {
        return m_patcherSecret;
    }

    QString applicationSecret() const
    {
        return m_applicationSecret;
    }

    QByteArray encodedPatcherSecret() const
    {
        return m_encodedPatcherSecret;
    }

    QByteArray encodedApplicationSecret() const
    {
        return m_encodedApplicationSecret;
    }

private:
    QString m_patcherSecret;
    QString m_applicationSecret;
    QByteArray m_encodedPatcherSecret;
    QByteArray m_encodedApplicationSecret;

    LauncherData(const QByteArray& t_encodedPatcherSecret, const QByteArray& t_encodedApplicationSecret);

    static QByteArray readStringBytes(QDataStream& t_fileStream);
    static QString decodeSecret(const QByteArray& t_encodedSecret);
};

#endif // LAUNCHERDATA_H
