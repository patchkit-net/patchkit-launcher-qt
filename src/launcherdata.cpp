#include "launcherdata.h"
#include "launcherexception.h"
#include <qdatastream.h>
#include <qfile.h>

LauncherData LauncherData::loadFromFile(const QString &fileName)
{
    QFile file(fileName);

    if(!file.open(QFile::ReadOnly))
    {
        throw LauncherException("Couldn't open launcher data file.");
    }

    LauncherData data;

    try
    {
        QDataStream fileStream(&file);

        fileStream.setByteOrder(QDataStream::LittleEndian);

        data.patcherSecret = readAndDecodeString(fileStream);
        data.gameSecret = readAndDecodeString(fileStream);
    }
    catch(...)
    {
        file.close();
        throw;
    }
    file.close();

    return data;
}

QString LauncherData::readAndDecodeString(QDataStream &fileStream)
{
    qint32 len;
    fileStream >> len;

    std::auto_ptr<char> bytes(new char[len]);

    fileStream.readRawData(bytes.get(), len);

    QString result = decodeString(bytes.get(), len);
    return result;
}

QString LauncherData::decodeString(const char *bytes, const int &len)
{
    std::auto_ptr<char> temp(new char[len]);

    memcpy(temp.get(), bytes, len);
    for(int i = 0; i < len; i++)
    {
        char b = temp.get()[i];
        bool lsb = (b & 1) > 0;
        b = b >> 1;
        b = b | (lsb ? 128 : 0);
        b = (char) ~b;
        temp.get()[i] = b;
    }

    return QString::fromUtf16(reinterpret_cast<const ushort*>(temp.get()), len/2);
}
