/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QtNetwork/QtNetwork>

#include <memory>
#include "remotepatcher.h"

class PatchKitRemotePatcher : public RemotePatcher
{
    Q_OBJECT

public:
    PatchKitRemotePatcher();

    int getVersion(const LauncherData& t_data) override;

    QString download(const LauncherData& t_data, int t_version) override;

public slots:
    void cancel() override;

signals:
    void cancelled();

private:
    QStringList getContentUrls(const QString& t_patcherSecret, int t_version) const;

    static int parseVersionJson(const QString& t_json);

    static QStringList parseContentUrlsJson(const QString& t_json);

    QString downloadString(const QString& t_urlPath) const;

    void downloadFile(const QString& t_filePath, const QString& t_urlPath) const;

    void waitForFileDownload(std::shared_ptr<QNetworkReply>& t_reply) const;

    static void writeDownloadedReplyToFile(std::shared_ptr<QNetworkReply>& t_reply, const QString& t_filePath);

    void getNetworkReply(const QString& t_urlPath,
                         std::shared_ptr<QNetworkAccessManager>& t_accessManager,
                         std::shared_ptr<QNetworkReply>& t_reply) const;

    void waitForNetworkReply(std::shared_ptr<QNetworkReply>& t_reply) const;

    static void validateNetworkReply(std::shared_ptr<QNetworkReply>& t_reply);


    bool m_isCancelled;

    const int downloadTimeoutInSeconds = 10;
};
