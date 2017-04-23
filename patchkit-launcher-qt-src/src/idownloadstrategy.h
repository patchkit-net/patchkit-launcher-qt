/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

class IDownloadStrategy
{
public:
    virtual void init() = 0;
    virtual void finish() = 0;
};
