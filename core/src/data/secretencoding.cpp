#include "secretencoding.h"
#include "customexceptions.h"

#include <QTextCodec>

QString data::secret::decode(QByteArray encodedSecret)
{
    for (int i = 0; i < encodedSecret.size(); i++)
    {
        char b = encodedSecret[i];
        bool lsb = (b & 1) > 0;

        b = b >> 1;
        b = b | (lsb ? 128 : 0);
        b = ~b;

        encodedSecret[i] = b;
    }

    auto raw = reinterpret_cast<const ushort*>(encodedSecret.data());
    return QString::fromUtf16(raw, encodedSecret.size() / 2);
}

QByteArray data::secret::encode(QString decodedSecret)
{
    const char* raw = reinterpret_cast<const char*>(decodedSecret.utf16());
    int size = decodedSecret.size() * 2;

    QByteArray encoded(raw, static_cast<int>(size));

    for (int i = 0; i < size; i++)
    {
        char b = ~encoded[i];
        bool fsb = (b & 128) > 0;

        b = static_cast<char>(b << 1);
        b = b | (fsb ? 1 : 0);

        encoded[i] = b;
    }

    return encoded;
}
