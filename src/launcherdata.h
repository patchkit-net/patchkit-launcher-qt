#ifndef LAUNCHERDATA_H
#define LAUNCHERDATA_H

#include <qstring.h>
#include <qdatastream.h>

class LauncherData
{
public:
    static LauncherData loadFromFile(QString& fileName);

    QString patcherSecret;

    QString gameSecret;
private:
    static QString readAndDecodeString(QDataStream &fileStream);
    static QString decodeString(char *bytes, int len);
};

#endif // LAUNCHERDATA_H
