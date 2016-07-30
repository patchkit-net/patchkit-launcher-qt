#include "launcherexception.h"

LauncherException::LauncherException(const QString& message) :
    m_message(message.toStdString().c_str())
{

}

LauncherException::LauncherException(const char *message) : m_message(message)
{

}
