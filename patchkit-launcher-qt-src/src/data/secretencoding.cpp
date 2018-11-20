#include "secretencoding.h"
#include "customexceptions.h"

QString data::secret::decode(const QByteArray& encodedSecret)
{
    size_t encodedSize = static_cast<size_t>(encodedSecret.size());
    std::unique_ptr<char> temp(new char[encodedSize]);
    memcpy(temp.get(), encodedSecret.data(), encodedSize);

    for (int i = 0; i < encodedSecret.size(); i++)
    {
        char b = temp.get()[i];
        bool lsb = (b & 1) > 0;
        b = b >> 1;
        b = b | (lsb ? 128 : 0);
        b = static_cast<char>(~b);
        temp.get()[i] = b;
    }

    return QString::fromUtf16(reinterpret_cast<const ushort*>(temp.get()), encodedSecret.size() / 2);
}

QByteArray data::secret::encode(const QString& /*decodedSecret*/)
{
    throw NotImplementedException("data::secret::encode");
}
