/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include <QCoreApplication>

#include <QThread>

#define CATCH_IMPL
#include "catch.h"

class Tester : public QThread
{
private:
    void run() override
    {
        Catch::Session().run();

        QCoreApplication::exit(0);
    }
};

int main (int argc, char * argv[])
{
    QCoreApplication app(argc, argv);

    Tester testerThread;
    testerThread.start();

    return app.exec();
}
