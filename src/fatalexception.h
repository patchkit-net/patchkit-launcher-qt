/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include "exception.h"

class FatalException : public Exception
{
public:
    FatalException(const QString& t_message) :
        Exception(t_message)
    {
    }

    FatalException(const char* t_message) :
        Exception(t_message)
    {
    }
};
