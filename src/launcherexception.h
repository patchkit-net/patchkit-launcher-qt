#ifndef LAUNCHEREXCEPTION_H
#define LAUNCHEREXCEPTION_H

#include <QException>

class LauncherException : public QException
{
public:
    LauncherException(const QString& t_message);
    LauncherException(const char* t_message);

    const char* what() const throw() override
    { return m_message.c_str(); }
    void raise() const override
    { throw *this; }
    LauncherException *clone() const override
    { return new LauncherException(*this); }

private:
    const std::string m_message;
};

#endif // LAUNCHEREXCEPTION_H
