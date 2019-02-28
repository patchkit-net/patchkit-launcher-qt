#include "downloadrange.h"

data::DownloadRange::DownloadRange(long long start, long long end)
    : start(start)
    , end(end)
{
}

QString data::DownloadRange::toString() const
{
    return QString("bytes=%1-%2").arg(QString::number(start), end == -1 ? "" : QString::number(end));
}
