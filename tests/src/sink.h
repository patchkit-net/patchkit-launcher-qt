#pragma once

#include <QIODevice>

class Sink : public QIODevice
{
protected:
    virtual qint64 readData(char* data, qint64 maxSize) override;
    virtual qint64 writeData(const char* data, qint64 maxSize) override;
};
