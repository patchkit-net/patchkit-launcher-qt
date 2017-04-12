/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

class ContentSummary;
class QString;
class QStringList;

class IApi
{
public:
    virtual ContentSummary  downloadContentSummary(const QString& t_resourceUrl) = 0;
    virtual QString         downloadPatcherSecret(const QString& t_resourceUrl) = 0;
    virtual int             downloadPatcherVersion(const QString& t_resourceUrl) = 0;
    virtual QStringList     downloadContentUrls(const QString& t_resourceUrl) = 0;
};
