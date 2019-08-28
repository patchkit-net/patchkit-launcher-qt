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
#include "locations/launcher.h"
#include "cancellation/cancellationtoken.h"
#include "ilauncher.h"

#include <quazipfile.h>
#include <memory>

class LocalPatcherData : public QObject
{
    Q_OBJECT

public:
    LocalPatcherData(const locations::Launcher& locations);

    bool isInstalled() const;
    bool isInstalledSpecific(int t_version, const Secret& patcherSecret);

    InstallationInfo install(
            const QString& t_downloadedPath, const Secret& patcherSecret, int t_version, CancellationToken cancellationToken);

    InstallationInfo install(
            QIODevice& source, const Secret& patcherSecret, int version, CancellationToken cancellationToken);

    void start(const Secret& appSecret, std::shared_ptr<LockFile> lockFile, data::NetworkStatus networkStatus);

private:
    void uninstall();

    int readVersion();

    const locations::Launcher& m_locations;

    static QString getPatcherId(const Secret& secret);

    static int parseVersionInfoToNumber(const QString& t_versionInfoFileContents);

    InstallationInfo readInstallationInfo() const;
    PatcherManifest readPatcherManifset() const;
};
