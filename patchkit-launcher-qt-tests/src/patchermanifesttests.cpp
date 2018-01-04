/*
* Copyright (C) Upsoft 2016
* License: https://github.com/patchkit-net/patchkit-launcher-qt/blob/master/LICENSE
*/

#include "catch.h"

#include <src/patchermanifest.h>

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
