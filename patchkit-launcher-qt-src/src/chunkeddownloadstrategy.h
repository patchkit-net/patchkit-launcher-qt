#ifndef CHUNKEDDOWNLOADSTRATEGY_H
#define CHUNKEDDOWNLOADSTRATEGY_H

#include "defaultdownloadstrategy.h"

class ChunkedDownloader;

class ChunkedDownloadStrategy : public DefaultDownloadStrategy
{
    Q_OBJECT
public:
    ChunkedDownloadStrategy(int t_minTimeout, int t_maxTimeout, const ChunkedDownloader& t_parent);

    virtual void finish() override;

protected:
    virtual void onDownloaderFinished(Downloader* downloader) override;

private:
    const ChunkedDownloader& m_parent;
    void setRanges(int t_from);
};

#endif // CHUNKEDDOWNLOADSTRATEGY_H
