#ifndef ILAUNCHERWINDOW_H
#define ILAUNCHERWINDOW_H

#include <qstring.h>

class ILauncherWindow
{
public:
    virtual void setBytesDownloaded(long bytesDownloaded) = 0;
    virtual void setTotalBytes(long totalBytes) = 0;
    virtual void setStatus(const QString &status) = 0;
    virtual void setProgress(int progress) = 0;
};

#endif // ILAUNCHERWINDOW_H
