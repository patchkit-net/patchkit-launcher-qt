/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "catch.h"

#include <data/patchermanifest.h>
#include <customexceptions.h>
#include <utilities.h>

#include <QJsonDocument>

const std::string patcherManifestData =
R"(
{
    "exe_fileName" : "\"{exedir}/Patcher\"",
    "exe_arguments" : "--installdir \"{installdir}\" --secret \"{secret}\"",

    "manifest_version": 2,
    "target": "open",
    "target_arguments": [
        { "value": ["{exedir}/patcher.app"] },
        { "value": ["--args"] },
        { "value": ["--installdir", "{installdir}"] },
        { "value": ["--lockfile", "{lockfile}"] },
        { "value": ["--secret", "{secret}"]}
    ]
}
)";

const std::string invalidPatcherManifestData =
R"(
{
    "exe_fileName" : "\"{exedir}/Patcher\"",
    "exe_arguments" : "--installdir \"{installdir}\" --secret \"{secret}\"",

    "manifest_version": 2,
    "target_arguments": [
        { "value": ["{exedir}/patcher.app"] },
        { "value": ["--args"] },
        { "value": ["--installdir", "{installdir}"] },
        { "value": ["--lockfile", "{lockfile}"] },
        { "value": ["--secret", "{secret}"]}
    ]
}
)";

TEST_CASE("Patcher manifest JSON parsing")
{
    PatcherManifestContext context({
        std::make_pair("{secret}", "SECRET"),
        std::make_pair("{exedir}", "HERE")
    });

    SECTION("Valid jason data")
    {
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(patcherManifestData));
        PatcherManifest manifest;

        REQUIRE_NOTHROW(manifest = Utilities::parsePatcherManifest(doc));

        REQUIRE(manifest.version() == 2);
        REQUIRE(manifest.makeTarget(context).toStdString() == "open");
        REQUIRE(manifest.makeArguments(context).join(" ").toStdString() == "HERE/patcher.app --args --secret SECRET");
    }

    SECTION("Invalid json data")
    {
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(invalidPatcherManifestData));
        REQUIRE_THROWS(Utilities::parsePatcherManifest(doc));
    }
}


TEST_CASE("Patcher manifest symbol detection")
{
    QString textWithNoSymbols = "The quick brown fox jumps over the lazy dog";
    QString textWithOneSymbol = "The quick brown fox {jumps} over the lazy dog";
    QString textWithTwoSymbols = "The {quick} brown fox {jumps} over the lazy dog";

    QString singleSymbolText = "{symbol}";

    REQUIRE(PatcherManifest::containsSymbols(textWithNoSymbols) == false);
    REQUIRE(PatcherManifest::containsSymbols(textWithOneSymbol) == true);
    REQUIRE(PatcherManifest::containsSymbols(textWithTwoSymbols) == true);
    REQUIRE(PatcherManifest::containsSymbols(singleSymbolText) == true);

    REQUIRE(PatcherManifest::isSymbol(singleSymbolText) == true);
    REQUIRE(PatcherManifest::isSymbol(textWithNoSymbols) == false);
    REQUIRE(PatcherManifest::isSymbol(textWithOneSymbol) == false);
}

TEST_CASE("Patcher manifest context construction")
{
    PatcherManifestContext context({
        std::make_pair("{test_symbol_1}", "TEST1"),
        std::make_pair("{test_symbol_2}", "TEST2")
    });

    REQUIRE(context.symbols().size() == 2);

    REQUIRE(context.symbols().at(0).toStdString() == "{test_symbol_1}");
    REQUIRE(context.symbols().at(1).toStdString() == "{test_symbol_2}");

    REQUIRE(context.symbolValue("{test_symbol_1}").toStdString() == "TEST1");
    REQUIRE(context.symbolValue("{test_symbol_2}").toStdString() == "TEST2");
}

TEST_CASE("Arguments parsing")
{
    PatcherManifestContext context({
        std::make_pair("{target_symbol}", "TARGET_TEST"),
        std::make_pair("{test_symbol_1}", "TEST1"),
        std::make_pair("{test_symbol_2}", "TEST2")
    });

    PatcherManifest manifest(2, "{target_symbol}/target", {
        {"--prefix"},
        {"--opt-test", "{test_symbol_1}"},
        {"--missing-opt", "{test_symbol_3}"},
        {"--postfix"}
    });

    QStringList expectedArguments = {"--prefix", "--opt-test", "TEST1", "--postfix"};
    auto expectedTarget = "TARGET_TEST/target";
    int expectedVersion = 2;

    auto version = manifest.version();
    auto target = manifest.makeTarget(context).toStdString();
    auto targetArguments = manifest.makeArguments(context).join(" ").toStdString();

    REQUIRE(targetArguments == expectedArguments.join(" ").toStdString());
    REQUIRE(target == expectedTarget);
    REQUIRE(version == expectedVersion);
}
