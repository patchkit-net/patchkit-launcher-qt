/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>
#include <QString>
#include <QIODevice>

#include "data/data.h"
#include "data/patchermanifest.h"
#include "data/networkstatus.hpp"
#include "data/installationinfo.h"
#include "lockfile.h"
#include "locations.h"
#include "cancellation/cancellationtoken.h"

#include <quazipfile.h>

class LocalPatcherData : public QObject
{
    Q_OBJECT

public:
    LocalPatcherData(const Locations& locations);

    bool isInstalled() const;
    bool isInstalledSpecific(int t_version, const Data& t_data);

    InstallationInfo install(
            const QString& t_downloadedPath, const Data& t_data, int t_version, CancellationToken cancellationToken);

    InstallationInfo install(
            QIODevice& source, const Data& data, int version, CancellationToken cancellationToken);

    void start(const Data& t_data, data::NetworkStatus networkStatus);

private:
    void uninstall();

    int readVersion();

    const Locations& m_locations;

    static QString getPatcherId(const Data& t_data);

    static int parseVersionInfoToNumber(const QString& t_versionInfoFileContents);

    InstallationInfo readInstallationInfo() const;
    PatcherManifest readPatcherManifset() const;
};
