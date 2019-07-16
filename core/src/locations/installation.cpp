#include "installation.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDir>

#include "customexceptions.h"
#include "config.h"
#include "ioutils.h"

static const QString patcherInstallationLocationKey = "patcherInstallationLocation";
static const QString appInstallationLocationKey = "appInstallationLocation";

locations::Installation locations::Installation::load(const QString& workingDir)
{
    QString path = QDir(workingDir).filePath(Config::locationFileName);
    QFile documentFile(path);
    if (!documentFile.open(QFile::ReadOnly))
    {
        throw IOError(path);
    }

    if (!documentFile.exists())
    {
        throw FileNotFound(path);
    }

    QByteArray data = documentFile.readAll();
    QJsonDocument document = QJsonDocument::fromJson(data);

    if (!document.isObject())
    {
        throw InvalidFormatException("Installation info file must be object");
    }

    QJsonObject root = document.object();

    if (!root.contains(patcherInstallationLocationKey))
    {
        throw InvalidFormatException("Installation info file must contain patcher installation location");
    }

    if (!root.contains(appInstallationLocationKey))
    {
        throw InvalidFormatException("Installation info file must containt app installation location");
    }

    // TODO: Error checking - both values must be string
    QString patcherInstallationLocation = root[patcherInstallationLocationKey].toString();
    QString appInstallationLocation = root[appInstallationLocationKey].toString();

    return Installation(patcherInstallationLocation, appInstallationLocation);
}

bool locations::Installation::canLoad(const QString& workingDir)
{
    QString sourceFile = QDir(workingDir).filePath(Config::locationFileName);

    return QFile::exists(sourceFile);
}

void locations::Installation::save(const Installation& installation, const QString& workingDir)
{
    QString path = QDir(workingDir).filePath(Config::locationFileName);

    QJsonObject root;
    root[patcherInstallationLocationKey] = installation.patcherLocation();
    root[appInstallationLocationKey] = installation.appLocation();

    QJsonDocument document;
    document.setObject(root);

    auto data =document.toJson();

    IOUtils::writeDataToFile(path, data);
}

locations::Installation::Installation()
{}

locations::Installation::Installation(const QString& patcherInstallationLocation, const QString& appInstallationLocation)
    : m_patcherInstallationLocation(patcherInstallationLocation)
    , m_appInstallationLocation(appInstallationLocation)
{}

QString locations::Installation::patcherLocation() const
{
    return m_patcherInstallationLocation;
}

QString locations::Installation::appLocation() const
{
    return m_appInstallationLocation;
}

void locations::Installation::save(const QString& path) const
{
    Installation::save(*this, path);
}
