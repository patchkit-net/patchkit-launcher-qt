#ifndef PATCHKITLOCALPATCHER_H
#define PATCHKITLOCALPATCHER_H

#include "localpatcher.h"

class PatchKitLocalPatcher : public LocalPatcher
{
public:
    int getVersion() const Q_DECL_OVERRIDE;
    bool isInstalled() const Q_DECL_OVERRIDE;
    void install(const QString &filePath, const int &version) const Q_DECL_OVERRIDE;
    void uninstall() const Q_DECL_OVERRIDE;
    void start(const QString &applicationSecret) const Q_DECL_OVERRIDE;
private:
    void readPatcherManifset(QString &exeFileName, QString &exeArguments) const;

    QString formatPatcherManifest(const QString& stringToFormat, const QString &applicationSecret) const;

    const QString installationDirectory = "../app";
    const QString patcherDirectory = "patcher";
    const QString installationInfoFileName = "install_info";
    const QString versionInfoFileName = "version_info";
    const QString manifsetFileName = "patcher.manifest";
};

#endif // PATCHKITLOCALPATCHER_H
