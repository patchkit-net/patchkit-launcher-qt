#pragma once
#include <QIODevice>

class ProgressDevice : public QIODevice
{
    Q_OBJECT
public:
    ProgressDevice(QIODevice& downstream, qint64 size);

signals:
    void onProgress(qint64 bytes);

protected:
    qint64 readData(char* data, qint64 maxSize) override;
    qint64 writeData(const char* data, qint64 maxSize) override;

private:
    QIODevice& m_downstream;
    qint64 m_total;
    qint64 m_size;
};
