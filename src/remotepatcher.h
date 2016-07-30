#ifndef IREMOTEPATCHER_H
#define IREMOTEPATCHER_H

#include <QObject>
#include <QString>

class RemotePatcher : public QObject
{
    Q_OBJECT

public:
    virtual int getVersion(const QString& patcherSecret) = 0;
    virtual QString download(const QString& patcherSecret, const int& version) = 0;

signals:
    void bytesDownloadedChanged(const long long& bytesDownloaded);
    void totalBytesChanged(const long long& totalBytes);
};

#endif // IREMOTEPATCHER_H
