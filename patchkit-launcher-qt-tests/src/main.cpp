/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#define CATCH_IMPL
#include "catch.h"

#include <QCoreApplication>

#include <QThread>

// TESTS
#include "contentsummarytests.h"
#include "remotepatcherdatatests.h"
#include "downloadertests.h"
#include "hashingstrategiestests.h"

class Tester : public QThread
{
public:

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
