/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include <QCoreApplication>
#include <QThread>

#include <iostream>
#include <sstream>

#include "src/logger.h"

namespace Catch
{
    std::stringstream testOutputStream;
    std::stringstream testErrorStream;

    std::ostream& cout()
    {
        return testOutputStream;
    }

    std::ostream& cerr()
    {
        return testErrorStream;
    }
}

#define CATCH_IMPL
#define CATCH_CONFIG_NOSTDOUT
#include "catch.h"

class Tester : public QThread
{
private:
    void run() override
    {
        Catch::Session().run();

        QCoreApplication::quit();
    }
};

int main (int argc, char * argv[])
{
    QCoreApplication app(argc, argv);

    Logger::getInstance().setSilent(true);

    Tester testerThread;
    testerThread.start();

    auto appExecResult = app.exec();

    std::cerr << Catch::testOutputStream.str();
    std::cerr << Catch::testErrorStream.str();

    return appExecResult;
}
