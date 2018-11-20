#pragma once
#include <QString>

class ILauncherInterface
{
public:
    virtual ~ILauncherInterface() {}

    virtual bool shouldRetry(QString reason) = 0;
    virtual int  ask(QString question, QString optionA, QString optionB) = 0;
};
