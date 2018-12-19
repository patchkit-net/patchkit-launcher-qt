#pragma once
#include <QString>
#include <QByteArray>

namespace data
{
namespace secret
{
    QString decode(QByteArray encodedSecret);
    QByteArray encode(QString decodedSecret);
}
}
