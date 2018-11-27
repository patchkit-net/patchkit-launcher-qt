/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

class ContentSummary;
class QString;
class QStringList;

class CancellationToken;

class IApi
{
public:
    virtual ~IApi() {}

    virtual ContentSummary getContentSummary(
            const QString& appSecret, int versionId, CancellationToken cancellationToken) const = 0;

    virtual QString getPatcherSecret(
            const QString& appSecret, CancellationToken cancellationToken) const = 0;

    virtual QString getDefaultPatcherSecret(
            CancellationToken cancellationToken
            ) const = 0;

    virtual int getLatestAppVersion(
            const QString& appSecret, CancellationToken cancellationToken) const = 0;

    virtual QStringList getContentUrls(
            const QString& appSecret, int versionId, CancellationToken cancellationToken) const = 0;
};
