#pragma once

#include <exception>

#define CUSTOM_EXCEPTION(classname) \
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

CUSTOM_EXCEPTION(FatalException)
CUSTOM_EXCEPTION(ContentUnavailableException)
