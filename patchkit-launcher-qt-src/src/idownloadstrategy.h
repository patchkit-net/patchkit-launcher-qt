#ifndef IDOWNLOADSTRATEGY_H
#define IDOWNLOADSTRATEGY_H

class IDownloadStrategy
{
public:
    virtual void init() = 0;
    virtual void finish() = 0;
};

#endif // IDOWNLOADSTRATEGY_H
