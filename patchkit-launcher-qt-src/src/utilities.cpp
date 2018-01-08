/*
* Copyright (C) Upsoft 2017
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "utilities.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDebug>

#include "locations.h"
#include "config.h"
#include "customexceptions.h"
#include "patchermanifest.h"
#include "ioutils.h"

#if defined(Q_OS_WIN)
#include <Windows.h>
#endif

void Utilities::tryRestartWithHigherPermissions()
{
#if defined(Q_OS_WIN)
    ShellExecute(nullptr, L"runas", Locations::getInstance().applicationFilePath().toStdWString().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    throw CancelledException();
#else
    throw FatalException("Cannot write in current directory.");
#endif
}

bool Utilities::isCurrentDirectoryWritable()
{
    return Locations::getInstance().isCurrentDirWritable();
}

PatcherManifest Utilities::parsePatcherManifest(const QString& t_filename)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(IOUtils::readTextFromFile(t_filename).toUtf8());
    return parsePatcherManifest(jsonDocument);
}

PatcherManifest Utilities::parsePatcherManifest(const QJsonDocument& t_document)
{
    if (t_document.isEmpty() || t_document.isNull())
    {
        throw InvalidFormatException("Document is null or empty.");
    }

    if (!t_document.isObject())
    {
        throw InvalidFormatException("Root of document is not an object.");
    }

    QJsonObject docObject = t_document.object();

    if (!docObject.contains(PatcherManifest::targetToken))
    {
        throw InvalidFormatException("Couldn't resolve the target token field.");
    }

    QString target = docObject[PatcherManifest::targetToken].toString();

    if (!docObject.contains(PatcherManifest::manifestVersionToken))
    {
        throw InvalidFormatException("Couldn't resolve the version field.");
    }

    int version = docObject[PatcherManifest::manifestVersionToken].toInt();

    if (!docObject.contains(PatcherManifest::targetArgumentsToken))
    {
        throw InvalidFormatException("Couldn't resolve the target arguments field.");
    }

    QJsonArray targetArgumentsArray = docObject[PatcherManifest::targetArgumentsToken].toArray();
    if (targetArgumentsArray == QJsonArray())
    {
        throw InvalidFormatException("Target arguments field is not an array.");
    }


    QList<QStringList> arguments;
    for (auto arg : targetArgumentsArray)
    {
        QStringList argValues;
        if (!arg.isObject())
        {
            throw InvalidFormatException("Argument in target arguments array is not an object.");
        }

        auto argObject = arg.toObject();

        if (!argObject.contains(PatcherManifest::targetArgumentValueToken))
        {
            throw InvalidFormatException("Argument didn't contain the value field.");
        }

        auto valueArray = argObject[PatcherManifest::targetArgumentValueToken].toArray();
        if (valueArray == QJsonArray())
        {
            throw InvalidFormatException("The argument's value field wasn't an array.");
        }

        for (auto value : valueArray)
        {
            if (!value.isString())
            {
                throw InvalidFormatException("Couldn't resolve argument's value field elements as string.");
            }

            argValues.append(value.toString());
        }

        arguments.append(argValues);
    }

    return PatcherManifest(version, target, arguments);
}
