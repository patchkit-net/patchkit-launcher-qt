/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <exception>

class StaleDownloadException : public std::exception
{
public:
    StaleDownloadException()
    {
    }

    virtual ~StaleDownloadException() throw()
    {
    }

    virtual const char* what() const throw()
    {
        return "Stale download.";
    }

};
