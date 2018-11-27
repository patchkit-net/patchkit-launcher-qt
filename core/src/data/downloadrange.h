#pragma once
#include <QString>

namespace data
{
struct DownloadRange
{
    DownloadRange(long long int start, long long int end = -1);

    long long int start;
    long long int end;

    QString toString() const;
};
}
