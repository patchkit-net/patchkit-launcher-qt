#include "launcherexception.h"

LauncherException::LauncherException(const QString& t_message) :
    m_message(t_message.toStdString())
{

}

LauncherException::LauncherException(const char *t_message) : m_message(t_message)
{

}
