/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <exception>

#define CUSTOM_RUNTIME_ERROR(classname) \
class classname : public std::runtime_error \
{ \
public: \
    classname(const QString& t_message) :\
        std::runtime_error(t_message.toStdString())\
    {\
    }\
    classname(const char* t_message) : \
        std::runtime_error(t_message)\
    { \
    } \
}; \

#define CUSTOM_EXCEPTION(classname, msg) \
class classname : public std::exception \
{ \
public: \
    classname() \
    { \
    } \
    virtual ~classname() throw () \
    { \
    } \
    virtual const char* what() const throw () \
    { \
        return msg; \
    } \
}; \

CUSTOM_RUNTIME_ERROR(FatalException)
CUSTOM_RUNTIME_ERROR(ContentUnavailableException)
CUSTOM_EXCEPTION(TimeoutException, "Timeout.")
CUSTOM_EXCEPTION(CancelledException, "Operation has been cancelled.")
CUSTOM_EXCEPTION(LockException, "Lock file detected.")
