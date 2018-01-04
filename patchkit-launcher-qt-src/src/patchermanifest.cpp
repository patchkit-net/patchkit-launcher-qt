/*
* Copyright (C) Upsoft 2018
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "patchermanifest.h"

#include <QRegularExpression>
#include <QDebug>

#include <iostream>

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

PatcherManifest::PatcherManifest(
        int t_version,
        const QString& t_target,
        const QStringList& t_arguments,
        const PatcherManifest::TTokenList& t_tokens)
    : m_version(t_version)
    , m_target(t_target)
    , m_arguments(t_arguments)
    , m_tokens(t_tokens)
{
}

PatcherManifest::PatcherManifest(const QJsonDocument& t_document)
{

}

QStringList PatcherManifest::makeArguments(const PatcherManifestContext& t_context) const
{
    QStringList argList;

    for (auto arg : m_arguments)
    {
        if (isToken(arg))
        {
            argList.append(parseToken(m_tokens.at(arg), t_context));
        }
        else
        {
            argList.append(arg);
        }
    }

    return argList;
}

QString PatcherManifest::makeTarget(const PatcherManifestContext& t_context) const
{
    return parseSymbols(m_target, t_context);
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

bool PatcherManifest::isToken(const QString& t_text)
{
    return t_text.size() > 0 && t_text.startsWith(">") && t_text.endsWith("<");
}

QStringList PatcherManifest::parseToken(const QStringList& t_tokenRaw, const PatcherManifestContext& t_context)
{
    QStringList result;
    for (auto s : t_tokenRaw)
    {
        if (containsSymbols(s))
        {
            result.append(parseSymbols(s, t_context));
        }
        else
        {
            result.append(s);
        }
    }

    // Only parse if at least one symbol was parsed
    for (int i = 0; i < t_tokenRaw.size(); i++)
    {
        if (result.at(i) != t_tokenRaw.at(i))
        {
            return result;
        }
    }

    return {};
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
