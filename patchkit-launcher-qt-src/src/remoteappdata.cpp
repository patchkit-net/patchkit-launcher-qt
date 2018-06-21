#include "remoteappdata.h"

#include "customexceptions.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

int RemoteAppData::parseVersion(const QByteArray& t_data)
{
    const QString ID = "id";
    const QString ERROR_MESSAGE = "Couldn't read version id from JSON data.";

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

QStringList RemoteAppData::parseDefaultPatchers(const QByteArray& t_data)
{
    auto jsonDocument = QJsonDocument::fromJson(t_data);

    if (!jsonDocument.isArray())
    {
        throw std::runtime_error("Couldn't read default patchers from JSON data.");
    }

    auto jsonArray = jsonDocument.array();

    if (jsonArray.size() == 0)
    {
        throw std::runtime_error("Empty patcher list.");
    }

    std::vector<std::pair<QString, QString>> availableDefaultPatchers;

    for (int i = 0; i < jsonArray.size(); i++)
    {
        if (!jsonArray[i].isObject())
        {
            throw std::runtime_error("Unexpected value in the array.");
        }

        auto object = jsonArray[i].toObject();

        if (!(object.contains("platform") && object.contains("secret")))
        {
            throw std::runtime_error("Unexpected values in array object.");
        }

        if (!object["platform"].isString())
        {
            throw std::runtime_error("Platform value is not string.");
        }

        if (!object["secret"].isString())
        {
            throw std::runtime_error("Secret value is not string.");
        }

        auto defaultPatcher = std::make_pair(object["platform"].toString(), object["secret"].toString());
        availableDefaultPatchers.push_back(defaultPatcher);
    }

    for (auto defaultPatcher : availableDefaultPatchers)
    {
        if (defaultPatcher.first == Globals::toString(Globals::currentPlatform()))
        {
            return defaultPatcher.second;
        }
    }

    throw std::runtime_error("Couldn't resolve a default patcher for current platform.");
}
