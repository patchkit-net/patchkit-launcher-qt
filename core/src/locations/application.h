#pragma once

#include <QString>

namespace locations
{
    class Application
    {
    public:
        Application(const QString& applicationDirectory);

        QString directory() const;
    private:
        QString m_directory;
    };
} // namespace locations
