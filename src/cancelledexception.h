/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <exception>

class CancelledException : public std::exception
{
public:
    CancelledException()
    {
    }

    virtual ~CancelledException() throw ()
    {
    }

    virtual const char* what() const throw ()
    {
        return "Operation has been cancelled.";
    }
};
