/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

class IDownloadStrategy
{
public:
    virtual void startInternal() = 0;
    virtual void finishInternal() = 0;
};
