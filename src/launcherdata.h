#ifndef LAUNCHERDATA_H
#define LAUNCHERDATA_H

#include <QDataStream>

struct LauncherData
{
public:
    static LauncherData loadFromFile(const QString& t_fileName);

    QString patcherSecret;
    QString gameSecret;
private:
    static QString readAndDecodeString(QDataStream& t_fileStream);
    static QString decodeString(const char *t_bytes, int t_len);
};

#endif // LAUNCHERDATA_H
