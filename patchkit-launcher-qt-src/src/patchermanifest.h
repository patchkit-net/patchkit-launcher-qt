/*
* Copyright (C) Upsoft 2018
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#pragma once

#include <QString>
#include <QStringList>

#include <map>

class PatcherManifestContext
{
public:
    typedef std::map<QString, QString> TSymbols;

    PatcherManifestContext();
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
    const static QString manifestVersionToken;
    const static QString targetToken;
    const static QString targetArgumentsToken;
    const static QString targetArgumentValueToken;

    PatcherManifest();
    PatcherManifest(
            int t_version,
            const QString& t_target,
            const QList<QStringList>& t_arguments);

    QStringList makeArguments(const PatcherManifestContext& t_context) const;
    QString makeTarget(const PatcherManifestContext& t_context) const;

    int version() const;

    static bool isSymbol(const QString& t_text);
    static bool containsSymbols(const QString& t_text);

    static QStringList parseArgument(const QStringList& t_argument, const PatcherManifestContext& t_context);
    static QString parseSymbols(const QString& t_raw, const PatcherManifestContext& t_context);

private:
    int m_version;
    QString m_target;
    QList<QStringList> m_arguments;
};
