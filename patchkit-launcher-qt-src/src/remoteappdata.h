#pragma once

#include <QByteArray>
#include <QStringList>

#include "customexceptions.h"

namespace RemoteAppData
{
    CUSTOM_RUNTIME_ERROR(ParsingError)

    int parseVersion(const QByteArray& t_data);
    QString parsePatcherSecret(const QByteArray& t_data);
    QString parseDefaultPatchers(const QByteArray& t_data);
    QString parseGeolocation(const QByteArray& t_data);
    QString tryParseGeolocation(const QByteArray& t_data, const QString t_default);
}
