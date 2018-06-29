#include "remoteappdata.h"

#include "customexceptions.h"
#include "config.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>

int RemoteAppData::parseVersion(const QByteArray& t_data)
{
    const QString ID = "id";
    const QString ERROR_MESSAGE = "Couldn't read version id from JSON data.";

    qDebug("Parsing version from JSON data");
    QJsonDocument doc = QJsonDocument::fromJson(t_data);

    if (!doc.isObject())
    {
        throw ParsingError(ERROR_MESSAGE);
    }

    QJsonObject root = doc.object();

    if (!root.contains(ID))
    {
        throw ParsingError(ERROR_MESSAGE);
    }

    QJsonValue id = root[ID];

    if (!id.isDouble())
    {
        throw ParsingError(ERROR_MESSAGE);
    }

    return id.toInt();
}

QString RemoteAppData::parsePatcherSecret(const QByteArray& t_data)
{
    const QString PATCHER_SECRET = "patcher_secret";
    const QString ERROR_MESSAGE = "Couldn't read patcher secret from JSON data.";

    qDebug("Parsing patcher secret from JSON data.");
    QJsonDocument doc= QJsonDocument::fromJson(t_data);

    if (!doc.isObject())
    {
        throw ParsingError(ERROR_MESSAGE);
    }

    QJsonObject root = doc.object();

    if (!root.contains(PATCHER_SECRET))
    {
        throw ParsingError(ERROR_MESSAGE);
    }

    QJsonValue secret = root[PATCHER_SECRET];

    if (!secret.isString())
    {
        throw ParsingError(ERROR_MESSAGE);
    }

    return secret.toString();
}

QString RemoteAppData::parseDefaultPatchers(const QByteArray& t_data)
{
    qDebug("Parsing default patchers from JSON data.");
    auto jsonDocument = QJsonDocument::fromJson(t_data);

    if (!jsonDocument.isArray())
    {
        throw ParsingError("Couldn't read default patchers from JSON data.");
    }

    auto jsonArray = jsonDocument.array();

    if (jsonArray.size() == 0)
    {
        throw ParsingError("Empty patcher list.");
    }

    std::vector<std::pair<QString, QString>> availableDefaultPatchers;

    for (int i = 0; i < jsonArray.size(); i++)
    {
        if (!jsonArray[i].isObject())
        {
            throw ParsingError("Unexpected value in the array.");
        }

        auto object = jsonArray[i].toObject();

        if (!(object.contains("platform") && object.contains("secret")))
        {
            throw ParsingError("Unexpected values in array object.");
        }

        if (!object["platform"].isString())
        {
            throw ParsingError("Platform value is not string.");
        }

        if (!object["secret"].isString())
        {
            throw ParsingError("Secret value is not string.");
        }

        auto defaultPatcher = std::make_pair(object["platform"].toString(), object["secret"].toString());
        availableDefaultPatchers.push_back(defaultPatcher);
    }

    qDebug("Resolving default patcher by platform.");
    for (auto defaultPatcher : availableDefaultPatchers)
    {
        if (defaultPatcher.first == Globals::toString(Globals::currentPlatform()))
        {
            return defaultPatcher.second;
        }
    }

    throw ParsingError("Couldn't resolve a default patcher for current platform.");
}

QString RemoteAppData::parseGeolocation(const QByteArray& t_data)
{
    const QString ERROR_MESSAGE = "Failed to resolve geolocation code from data";
    if (t_data.isEmpty())
    {
        throw ParsingError(ERROR_MESSAGE);
    }

    qDebug("Parsing geolocation from JSON data.");
    auto jsonDocument = QJsonDocument::fromJson(t_data);

    if (!jsonDocument.isObject())
    {
        throw ParsingError(ERROR_MESSAGE);
    }

    auto root = jsonDocument.object();

    if (!root.contains("country"))
    {
        throw ParsingError(ERROR_MESSAGE);
    }

    auto countryValue = root.value("country");

    if (!countryValue.isString())
    {
        throw ParsingError(ERROR_MESSAGE);
    }

    auto stringValue = countryValue.toString();

    return stringValue;
}

QString RemoteAppData::tryParseGeolocation(const QByteArray& t_data, const QString t_default)
{
    try
    {
        return parseGeolocation(t_data);
    }
    catch (ParsingError& e)
    {
        qWarning() << e.what();
        return t_default;
    }
}
