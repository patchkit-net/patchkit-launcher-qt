#ifndef LAUNCHERDATA_H
#define LAUNCHERDATA_H

#include <QString>
#include <QDataStream>

struct LauncherData
{
public:
    static LauncherData loadFromFile(const QString& fileName);

    QString patcherSecret;
    QString gameSecret;
private:
    static QString readAndDecodeString(QDataStream &fileStream);
    static QString decodeString(const char *bytes, const int &len);
};

#endif // LAUNCHERDATA_H
