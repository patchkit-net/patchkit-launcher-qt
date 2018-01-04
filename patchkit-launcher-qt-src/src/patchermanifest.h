/*
* Copyright (C) Upsoft 2018
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QString>
#include <QStringList>
#include <QJsonDocument>

#include <map>

class PatcherManifestContext
{
public:
    typedef std::map<QString, QString> TSymbols;

    PatcherManifestContext(const TSymbols& t_symbols);

    void defineSymbol(const QString& t_name, const QString& t_value);
    QString symbolValue(const QString& t_name) const;

    QStringList symbols() const;

private:
    TSymbols m_symbols;
};

class PatcherManifest
{
public:
    typedef std::map<QString, QStringList> TTokenList;

    PatcherManifest(
            int t_version,
            const QString& t_target,
            const QStringList& t_arguments,
            const TTokenList& t_tokens);

    PatcherManifest(const QJsonDocument& t_document);

    QStringList makeArguments(const PatcherManifestContext& t_context) const;
    QString makeTarget(const PatcherManifestContext& t_context) const;

    static bool isSymbol(const QString& t_text);
    static bool containsSymbols(const QString& t_text);

    static bool isToken(const QString& t_text);

    static QStringList parseToken(const QStringList& t_tokenRaw, const PatcherManifestContext& t_context);
    static QString parseSymbols(const QString& t_raw, const PatcherManifestContext& t_context);

private:
    int m_version;
    QString m_target;
    QStringList m_arguments;
    TTokenList m_tokens;
};
