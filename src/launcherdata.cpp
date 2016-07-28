#include "launcherdata.h"
#include <qdatastream.h>
#include <qfile.h>

LauncherData LauncherData::loadFromFile(QString &fileName)
{
    QFile file(fileName);

    if(!file.open(QFile::ReadOnly))
    {
        qFatal("couldn't open launcher data file");
    }

    QDataStream fileStream(&file);

    fileStream.setByteOrder(QDataStream::LittleEndian);

    LauncherData data;

    data.patcherSecret = readAndDecodeString(fileStream);
    data.gameSecret = readAndDecodeString(fileStream);

    file.close();

    return data;
}

QString LauncherData::readAndDecodeString(QDataStream &fileStream)
{
    qint32 len;
    char* bytes;

    fileStream >> len;

    bytes = new char[len];

    fileStream.readRawData(bytes, len);

    return decodeString(bytes, len);
}

QString LauncherData::decodeString(char *bytes, int len)
{
    for(int i = 0; i < len; i++)
    {
        char b = bytes[i];
        bool lsb = (b & 1) > 0;
        b = b >> 1;
        b = b | (lsb ? 128 : 0);
        b = (char) ~b;
        bytes[i] = b;
    }

    return QString::fromUtf16(reinterpret_cast<const ushort*>(bytes), len);
}
