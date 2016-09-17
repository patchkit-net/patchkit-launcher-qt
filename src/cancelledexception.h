/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <exception>

class CancelledException : public std::exception
{
public:
    CancelledException() : exception("Operation has been cancelled.")
    {
    }
};
