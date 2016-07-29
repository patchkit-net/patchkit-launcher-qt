#ifndef PATCHKITREMOTEPATCHER_H
#define PATCHKITREMOTEPATCHER_H

#include <QObject>
#include "remotepatcher.h"

class PatchKitRemotePatcher : public RemotePatcher
{
public:
    PatchKitRemotePatcher();

    int getVersion(const QString &patcherSecret) Q_DECL_OVERRIDE;
    QString download(const QString &patcherSecret, const int &version) Q_DECL_OVERRIDE;

private:
    const int downloadTimeoutInSeconds = 10;

    QString downloadString(const QString &urlPath);
};

#endif // PATCHKITREMOTEPATCHER_H
