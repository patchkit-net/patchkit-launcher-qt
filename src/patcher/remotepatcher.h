/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QObject>
#include <QString>

#include "launcherdata.h"

class RemotePatcher : public QObject
{
    Q_OBJECT

public:
    virtual int getVersion(const LauncherData& t_data) = 0;

    virtual QString download(const LauncherData& t_data, int t_version) = 0;

public slots:
    // Cancels any above operation. If cancellation is successful, LauncherCancelledException is thrown by the operation.
    virtual void cancel() = 0;

signals:
    void downloadProgressChanged(const long long& t_bytesDownloaded, const long long& t_totalBytes);
};
