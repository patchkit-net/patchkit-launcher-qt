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

TEST_CASE("Token parsing")
{
    PatcherManifestContext context({
        std::make_pair("{test_symbol}", "TEST")
    });

    PatcherManifest::TTokenList tokenList;
    tokenList.insert(std::make_pair(">token1<", QStringList({"--symbol", "{test_symbol}"})));

    PatcherManifest manifest(1, "target",
        {
            "pos1", "--opt1", ">token1<"
        },
        tokenList
    );

    QStringList expectedArguments = {"pos1", "--opt1", "--symbol", "TEST"};

    REQUIRE(manifest.makeTarget(context) == "target");
    REQUIRE(manifest.makeArguments(context).size() > 0);
    REQUIRE(manifest.makeArguments(context).join(" ").toStdString() == expectedArguments.join(" ").toStdString());
}

TEST_CASE("Token parsing - missing symbol values and parsing symbols in target name")
{
    PatcherManifestContext context({
        std::make_pair("{symbol}", "TEST")
    });

    PatcherManifest::TTokenList tokenList;
    tokenList.insert(std::make_pair(">token1<", QStringList({"--symbol", "{test_symbol}"})));

    PatcherManifest manifest(1, "{symbol}/target/{symbol}",
        {
            "pos1", "--opt1", ">token1<"
        },
        tokenList
    );

    QStringList expectedArguments = {"pos1", "--opt1"};

    REQUIRE(manifest.makeTarget(context) == "TEST/target/TEST");
    REQUIRE(manifest.makeArguments(context).size() > 0);
    REQUIRE(manifest.makeArguments(context).join(" ").toStdString() == expectedArguments.join(" ").toStdString());
}
