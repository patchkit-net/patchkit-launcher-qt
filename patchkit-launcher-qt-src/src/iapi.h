/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include "cancellationtoken.h"

class ContentSummary;
class QString;
class QStringList;

class IApi
{
public:
    virtual QString getCountryCode(CancellationToken t_cancellationToken) const = 0;

    virtual QString getPatcherSecret(
            const QString& t_appSecret, CancellationToken t_cancellationToken) const = 0;

    virtual ContentSummary getContentSummary(
            const QString& t_appSecret, int t_version, CancellationToken t_cancellationToken) const = 0;

    virtual QString getDefaultPatcherSecret(
            CancellationToken t_cancellationToken) const = 0;

    virtual int getLatestVersionId(
            const QString& t_appSecret, CancellationToken t_cancellationToken) const = 0;

    virtual QStringList getContentUrls(
            const QString& t_appSecret, int t_version, CancellationToken t_cancellationToken) const = 0;
};
