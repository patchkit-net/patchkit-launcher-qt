/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <exception>
#include <string>

#include <QString>

#define CUSTOM_RUNTIME_ERROR(classname) \
class classname : public std::runtime_error \
{ \
public: \
    classname(const std::string& message) \
        : std::runtime_error(std::string(#classname) + std::string(": ") + std::string(message)) \
    {} \
    \
    classname(const QString& msg) : classname(msg.toStdString()) {} \
    \
    classname(const char* msg) : classname(std::string(msg)) {} \
}; \

#define CUSTOM_EXCEPTION(classname, msg) \
class classname : public std::runtime_error \
{ \
public: \
    classname() :\
        std::runtime_error(msg)\
    {\
    }\
}; \

CUSTOM_RUNTIME_ERROR(FatalException)
CUSTOM_RUNTIME_ERROR(InvalidFormatException)
CUSTOM_RUNTIME_ERROR(NotImplementedException)
CUSTOM_RUNTIME_ERROR(NotSupportedException)
CUSTOM_RUNTIME_ERROR(Timeout)
