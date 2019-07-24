#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "interface.h"
#include "launcherworker.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("my-copy-program");
    QCoreApplication::setApplicationVersion("1.0");

    CliInterface interface(app.applicationDirPath());
    LauncherWorker worker(interface);
    worker.start();
    return app.exec();
}
