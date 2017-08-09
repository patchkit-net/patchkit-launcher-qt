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
    virtual const QString&  getCountryCode() const = 0;
};
