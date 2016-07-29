#ifndef LAUNCHEREXCEPTION_H
#define LAUNCHEREXCEPTION_H

#include <QException>

class LauncherException : public QException
{
public:
    LauncherException(const QString& message);
    LauncherException(const char* message);

    const char* what() const { return m_message; }
    void raise() const { throw *this; }
    LauncherException *clone() const { return new LauncherException(*this); }

private:
    const char* m_message;
};

#endif // LAUNCHEREXCEPTION_H
