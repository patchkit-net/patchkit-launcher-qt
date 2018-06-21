#pragma once

#include <QByteArray>
#include <QStringList>

namespace RemoteAppData
{
    int parseVersion(const QByteArray& t_data);
    QString parsePatcherSecret(const QByteArray& t_data);
    QStringList parseDefaultPatchers(const QByteArray& t_data);
}
