/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QException>

class LauncherException : public QException
{
public:
    LauncherException(const QString& t_message) :
        m_message(t_message.toStdString())
    {
    }

    LauncherException(const char* t_message) :
        m_message(t_message)
    {
    }

    const char* what() const throw() override
    {
        return m_message.c_str();
    }

    LauncherException* clone() const override
    {
        return new LauncherException(*this);
    }

    void raise() const override
    {
        throw *this;
    }

private:
    const std::string m_message;
};
