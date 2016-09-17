/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <exception>

class FatalException : public std::runtime_error
{
public:
    FatalException(const QString& t_message) :
        std::runtime_error(t_message.toStdString())
    {
    }

    FatalException(const char* t_message) :
        std::runtime_error(t_message)
    {
    }
};
