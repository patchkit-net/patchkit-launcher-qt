/*
* Copyright (C) Upsoft 2018
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "patchermanifest.h"

#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDebug>

#include <iostream>

#include "customexceptions.h"

const QString PatcherManifest::manifestVersionToken = "manifest_version";
const QString PatcherManifest::targetToken = "target";
const QString PatcherManifest::targetArgumentsToken = "target_arguments";
const QString PatcherManifest::targetArgumentValueToken = "value";

PatcherManifestContext::PatcherManifestContext(const PatcherManifestContext::TSymbols& t_symbols)
    : m_symbols(t_symbols)
{}

void PatcherManifestContext::defineSymbol(const QString& t_name, const QString& t_value)
{
    m_symbols.insert(std::make_pair(t_name, t_value));
}

QString PatcherManifestContext::symbolValue(const QString& t_name) const
{
    try
    {
        return m_symbols.at(t_name);
    }
    catch (std::out_of_range&)
    {
        return "";
    }
}

QStringList PatcherManifestContext::symbols() const
{
    QStringList result;
    for (auto k : m_symbols)
    {
        result.append(k.first);
    }
    return result;
}

PatcherManifest::PatcherManifest()
{}

PatcherManifest::PatcherManifest(int t_version,
        const QString& t_target,
        const QList<QStringList>& t_arguments)
    : m_version(t_version)
    , m_target(t_target)
    , m_arguments(t_arguments)
{
}

PatcherManifest::PatcherManifest(const QJsonDocument& t_document)
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

    if (!docObject.contains(targetToken))
    {
        throw InvalidFormatException("Couldn't resolve the target token field.");
    }

    m_target = docObject[targetToken].toString();

    if (!docObject.contains(manifestVersionToken))
    {
        throw InvalidFormatException("Couldn't resolve the version field.");
    }

    m_version = docObject[manifestVersionToken].toInt();

    if (!docObject.contains(targetArgumentsToken))
    {
        throw InvalidFormatException("Couldn't resolve the target arguments field.");
    }

    QJsonArray targetArgumentsArray = docObject[targetArgumentsToken].toArray();
    if (targetArgumentsArray == QJsonArray())
    {
        throw InvalidFormatException("");
    }

    for (auto arg : targetArgumentsArray)
    {
        QStringList argValues;
        if (!arg.isObject())
        {
            throw InvalidFormatException("");
        }

        auto argObject = arg.toObject();

        if (!argObject.contains(targetArgumentValueToken))
        {
            throw InvalidFormatException("");
        }

        auto valueArray = argObject[targetArgumentValueToken].toArray();
        if (valueArray == QJsonArray())
        {
            throw InvalidFormatException("");
        }

        for (auto value : valueArray)
        {
            if (!value.isString())
            {
                throw InvalidFormatException("");
            }

            argValues.append(value.toString());
        }

        m_arguments.append(argValues);
    }

}

QStringList PatcherManifest::makeArguments(const PatcherManifestContext& t_context) const
{
    QStringList argList;

    for (auto arg : m_arguments)
    {
        argList.append(parseArgument(arg, t_context));
    }

    return argList;
}

QString PatcherManifest::makeTarget(const PatcherManifestContext& t_context) const
{
    return parseSymbols(m_target, t_context);
}

int PatcherManifest::version() const
{
    return m_version;
}

bool PatcherManifest::isSymbol(const QString& t_text)
{
    return t_text.size() > 0 && t_text.startsWith("{") && t_text.endsWith("}");
}

bool PatcherManifest::containsSymbols(const QString& t_text)
{
    QRegularExpression pattern("\\{.+\\}");
    return t_text.contains(pattern);
}

QStringList PatcherManifest::parseArgument(const QStringList& t_argument, const PatcherManifestContext& t_context)
{
    QStringList result;

    for (auto s : t_argument)
    {
        if (containsSymbols(s))
        {
            auto parsed = parseSymbols(s, t_context);

            if (containsSymbols(parsed))
            {
                qWarning("Launcher found an unrecognized symbol in patcher.manifest.");
                return {};
            }

            result.append(parsed);
        }
        else
        {
            result.append(s);
        }
    }

    return result;
}

QString PatcherManifest::parseSymbols(const QString& t_raw, const PatcherManifestContext& t_context)
{
    auto parsed = t_raw;
    for (auto s : t_context.symbols())
    {
        parsed = parsed.replace(s, t_context.symbolValue(s));
    }
    return parsed;
}
