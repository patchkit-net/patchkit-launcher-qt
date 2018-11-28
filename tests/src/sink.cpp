#include "sink.h"

qint64 Sink::readData(char* /*data*/, qint64 maxSize)
{
    return maxSize;
}

qint64 Sink::writeData(const char* /*data*/, qint64 maxSize)
{
    return maxSize;
}
