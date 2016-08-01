#ifndef PATCHKITREMOTEPATCHER_H
#define PATCHKITREMOTEPATCHER_H

#include <QtNetwork/QtNetwork>
#include "remotepatcher.h"

class PatchKitRemotePatcher : public RemotePatcher
{
    Q_OBJECT

public:
    PatchKitRemotePatcher();

    int getVersion(const QString& t_patcherSecret) Q_DECL_OVERRIDE;

    QString download(const QString& t_patcherSecret, int t_version) Q_DECL_OVERRIDE;

signals:
    void cancelled();

public slots:
    void cancel() Q_DECL_OVERRIDE;

private:
    const int downloadTimeoutInSeconds = 10;

    bool m_isCancelled;

    QStringList getContentUrls(const QString& t_patcherSecret, int t_version) const;

    void getNetworkReply(const QString& t_urlPath, std::auto_ptr<QNetworkAccessManager>& t_accessManager, std::auto_ptr<QNetworkReply>& t_reply) const;

    QString downloadString(const QString& t_urlPath) const;

    void downloadFile(const QString& t_filePath, const QString& t_urlPath) const;
};

#endif // PATCHKITREMOTEPATCHER_H
