/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include <QCoreApplication>

#include <QThread>

#include <iostream>

namespace Catch
{
    std::ostream& cout()
    {
        return std::cerr;
    }

    std::ostream& cerr()
    {
        return std::cerr;
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

    Tester testerThread;
    testerThread.start();

    return app.exec();
}
