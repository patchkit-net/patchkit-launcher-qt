/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#ifndef LAUNCHERDATA_H
#define LAUNCHERDATA_H

#include <QDataStream>

struct LauncherData
{
    LauncherData();

    static LauncherData loadFromFile(const QString& t_fileName);

#ifdef Q_OS_WIN
    static LauncherData loadFromResource(const QString& t_applicationFilePath, int t_resourceId, int t_resourceTypeId);
#endif

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
    static LauncherData loadFromDataStream(QDataStream& t_dataStream);

    QString m_patcherSecret;
    QString m_applicationSecret;
    QByteArray m_encodedPatcherSecret;
    QByteArray m_encodedApplicationSecret;

    LauncherData(const QByteArray& t_encodedPatcherSecret, const QByteArray& t_encodedApplicationSecret);

    static QByteArray readStringBytes(QDataStream& t_dataStream);
    static QString decodeSecret(const QByteArray& t_encodedSecret);
};

#endif // LAUNCHERDATA_H
