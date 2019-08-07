/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "launcher.h"
#include "cliinterface.h"

#include <logger.h>
#include <lockfile.h>
#include <config.h>

#include <QMessageBox>
#include <QNetworkProxyFactory>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QVariant>
#include <QProcessEnvironment>
#include <QScopedPointer>

void setEnv(const QString& key, const QString& value)
{
    qputenv(key.toStdString().c_str(), value.toUtf8());
}

QCoreApplication* createApplication(int &argc, char *argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        if (!qstrcmp(argv[i], "--cli"))
        {
            return new QCoreApplication(argc, argv);
        }

        if (!qstrcmp(argv[i], "-c"))
        {
            return new QCoreApplication(argc, argv);
        }
    }
    return new QApplication(argc, argv);
}

int main(int argc, char* argv[])
{
    QTextStream out(stdout);

    QScopedPointer<QCoreApplication> app(createApplication(argc, argv));

    QCoreApplication::setApplicationName("Launcher");
    QCoreApplication::setApplicationVersion(Globals::version());

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption noGuiOption({"c", "cli"}, "Disable Launcher GUI");
    parser.addOption(noGuiOption);

    QCommandLineOption cliShouldRetry(
                "cli-should-retry",
                "Override the should retry option in cli mode",
                "SHOULD_RETRY",
                "true");
    parser.addOption(cliShouldRetry);

    QCommandLineOption cliInstallationLocation(
                "cli-installation-location",
                "Override default installation location in cli mode",
                "INSTALLATION_LOCATION",
                QCoreApplication::applicationDirPath());
    parser.addOption(cliInstallationLocation);

    QCommandLineOption overridePatcherSecret({"p", "patcher-secret"}, "Override the patcher secret", "PATCHER_SECRET");
    parser.addOption(overridePatcherSecret);

    parser.process(QCoreApplication::arguments());

    if (parser.isSet(overridePatcherSecret))
    {
        setEnv(Config::patcherSecretOverrideEnvVar, parser.value(overridePatcherSecret));
    }

    if (parser.isSet(noGuiOption))
    {
        CliInterface interface(
                    parser.value(cliInstallationLocation),
                    ILauncherInterface::OfflineModeAnswer::CANCEL,
                    QVariant(parser.value(cliShouldRetry)).toBool());

        LauncherWorker worker(interface);
        worker.start();

        QObject::connect(&worker, &LauncherWorker::finished, app.data(), &QApplication::quit);

        return app->exec();
    }
    else
    {
        auto application = qobject_cast<QApplication*>(app.data());
        qInfo("Setting apllication window parameters.");
        application->setQuitOnLastWindowClosed(false);

        qInfo("Starting Launcher thread");
        LauncherApp::Launcher launcher(*application);
        launcher.start();

        qInfo("Starting application loop.");
        return app->exec();
    }
}
