/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include "exception.h"

class TimeoutException : public Exception
{
public:
    TimeoutException() : Exception("Timeout.")
    {
    }
};
