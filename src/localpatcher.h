#ifndef LOCALPATCHER_H
#define LOCALPATCHER_H

#include <QObject>
#include <QString>

class LocalPatcher : public QObject
{
    Q_OBJECT

public:
    virtual int getVersion() const = 0;
    virtual bool isInstalled() const = 0;
    virtual void install(const QString& t_filePath, int t_version) const = 0;
    virtual void uninstall() const = 0;
    virtual void start(const QString& t_applicationSecret) const =0;
};


#endif // LOCALPATCHER_H
