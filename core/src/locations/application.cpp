#include "application.h"

locations::Application::Application(const QString& applicationDirectory)
    : m_directory(applicationDirectory)
{
}

QString locations::Application::directory() const
{
    return m_directory;
}
