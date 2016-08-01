#ifndef IREMOTEPATCHER_H
#define IREMOTEPATCHER_H

#include <QObject>
#include <QString>

class RemotePatcher : public QObject
{
    Q_OBJECT

public:
    virtual int getVersion(const QString& t_patcherSecret) = 0;
    virtual QString download(const QString& t_patcherSecret, int t_version) = 0;
    
public slots:
    virtual void cancel() = 0;

signals:
    void downloadProgress(const long long& t_bytesDownloaded, const long long& t_totalBytes);
};

#endif // IREMOTEPATCHER_H
