/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#ifndef STALEDOWNLOADEXCEPTION_H
#define STALEDOWNLOADEXCEPTION_H

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

#endif // STALEDOWNLOADEXCEPTION_H
