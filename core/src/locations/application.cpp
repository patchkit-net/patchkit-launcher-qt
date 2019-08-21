#include "application.h"

#include "ioutils.h"

locations::Application::Application(const QString& applicationDirectory)
    : m_directory(applicationDirectory)
{
    IOUtils::createDir(m_directory);
}

QString locations::Application::directory() const
{
    return m_directory;
}
