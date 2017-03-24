/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>
#include <QString>

#include "data.h"
#include <quazipfile.h>

class LocalPatcherData : public QObject
{
    Q_OBJECT

public:
    bool isInstalled() const;

    bool isInstalledSpecific(int t_version, const Data& t_data);

    void install(const QString& t_downloadedPath, const Data& t_data, int t_version);

    void start(const Data& t_data);

private:
    void uninstall();

    int readVersion();

    static QString getPatcherId(const Data& t_data);

    static int parseVersionInfoToNumber(const QString& t_versionInfoFileContents);

    void readPatcherManifset(QString& t_exeFileName,
                             QString& t_exeArguments) const;

    QString formatPatcherManifestString(const QString& t_stringToFormat,
                                        const QByteArray& t_encodedApplicationSecret) const;
};
