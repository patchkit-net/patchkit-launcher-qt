/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

class IDownloadStrategy
{
protected:
    virtual void execute(CancellationToken t_cancellationToken) = 0;
};
