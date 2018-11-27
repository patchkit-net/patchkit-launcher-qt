#include "progressdevice.h"
#include "customexceptions.h"

ProgressDevice::ProgressDevice(QIODevice& downstream, qint64 size)
    : m_downstream(downstream)
    , m_size(size)
{
}

qint64 ProgressDevice::readData(char* data, qint64 maxSize)
{
    auto read = m_downstream.write(data, maxSize);
    m_total += read;

    emit onProgress(m_total);

    return read;
}

qint64 ProgressDevice::writeData(const char* data, qint64 maxSize)
{
    auto written = m_downstream.write(data, maxSize);
    m_total += written;

    emit onProgress(m_total);

    return written;
}
