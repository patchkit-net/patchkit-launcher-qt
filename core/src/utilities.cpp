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
#include <QDir>

#include "locations.h"
#include "config.h"
#include "customexceptions.h"
#include "data/patchermanifest.h"
#include "cancellation/cancellationtoken.h"
#include "ioutils.h"

#if defined(Q_OS_WIN)
#include <Windows.h>
#endif

void Utilities::tryRestartWithHigherPermissions()
{
#if defined(Q_OS_WIN)
    ShellExecute(nullptr, L"runas", Locations::applicationFilePath().toStdWString().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    throw CancellationToken::CancelledException();
#else
    throw FatalException("Cannot write in current directory.");
#endif
}

bool Utilities::isDirectoryWritable(const QString& dirPath)
{
    QFile permissionsCheckFile(QDir(dirPath).filePath(".writable_check"));

    if (permissionsCheckFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        permissionsCheckFile.remove();

        return true;
    }

    return false;
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

PatcherManifest Utilities::parseOldPatcherManifest(const QString& t_filename)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(IOUtils::readTextFromFile(t_filename).toUtf8());
    return parseOldPatcherManifest(jsonDocument);
}

PatcherManifest Utilities::parseOldPatcherManifest(const QJsonDocument& t_document)
{
    if (t_document.isEmpty() || t_document.isNull())
    {
        throw InvalidFormatException("Document is null or empty");
    }

    if (!t_document.isObject())
    {
        throw InvalidFormatException("Document root is not an object.");
    }

    auto root = t_document.object();

    if (!root.contains(PatcherManifest::exeFilenameToken))
    {
        throw InvalidFormatException("Couldn't resolve the exe filename field in document.");
    }

    if (!root.contains(PatcherManifest::exeArgumentsToken))
    {
        throw InvalidFormatException("Couldn't resolve the exe arguments field in document.");
    }

    if (!root[PatcherManifest::exeFilenameToken].isString())
    {
        throw InvalidFormatException("The exe filename field must be a string.");
    }

    if (!root[PatcherManifest::exeArgumentsToken].isString())
    {
        throw InvalidFormatException("The exe arguments field must be a string.");
    }

    auto exeFilename = root[PatcherManifest::exeFilenameToken].toString();
    auto exeArguments = root[PatcherManifest::exeArgumentsToken].toString();

    exeFilename.replace("\"", "");
    exeArguments.replace("\"", "");

    auto arguments = exeArguments.split(" ");
    return PatcherManifest(0, exeFilename, {arguments});
}
