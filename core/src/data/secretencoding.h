#pragma once
#include <QString>
#include <QByteArray>

namespace data
{
namespace secret
{
    QString decode(const QByteArray& encodedSecret);
    QByteArray encode(const QString& decodedSecret);
}
}
