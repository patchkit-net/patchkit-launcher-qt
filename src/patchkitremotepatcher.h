#ifndef PATCHKITREMOTEPATCHER_H
#define PATCHKITREMOTEPATCHER_H

#include <QtNetwork/QtNetwork>
#include <QObject>
#include "remotepatcher.h"

class PatchKitRemotePatcher : public RemotePatcher
{
public:
    int getVersion(const QString &patcherSecret) Q_DECL_OVERRIDE;

    QString download(const QString &patcherSecret, const int &version) Q_DECL_OVERRIDE;

private slots:
    void downloadProgress(const long long& bytesReceived, const long long &bytesTotal);

private:
    const int downloadTimeoutInSeconds = 10;

    QString *getContentUrls(const QString &patcherSecret, const int &version);

    void getNetworkReply(const QString &urlPath, QNetworkAccessManager*& accessManager, QNetworkReply*& reply);

    QString downloadString(const QString &urlPath);

    void downloadFile(const QString &filePath, const QString &urlPath);
};

#endif // PATCHKITREMOTEPATCHER_H
