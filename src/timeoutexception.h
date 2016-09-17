/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <exception>

class TimeoutException : public std::exception
{
public:
    TimeoutException() : std::exception("Timeout.")
    {
    }
};
