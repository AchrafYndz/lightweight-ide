#include "doctest/doctest.h"

#include "machines/CFG.h"

#include <fstream>
#include <sstream>

TEST_SUITE("CFGTests") {
    TEST_CASE("[CFGTests] parseFile0") {
        CFG cfg("test/res/input/CFG0.json");

        const std::ifstream expectedFile("test/res/expected/CFGTests-parseFile0.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        cfg.print(actual);

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[CFGTests] parseFile1") {
        CFG cfg("test/res/input/CFG1.json");

        const std::ifstream expectedFile("test/res/expected/CFGTests-parseFile1.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        cfg.print(actual);

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[CFGTests] LL") {
        CFG cfg("test/res/input/LL0.json");

        const std::ifstream expectedFile("test/res/expected/CFGTests-LL0.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        // std::ofstream reset("test/res/expected/CFGTests-LL0.txt");

        std::stringstream actual("");

        cfg.ll(actual);

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[CFGTests] CNF") {
        CFG cfg("test/res/input/CNF0.json");

        const std::ifstream expectedFile("test/res/expected/CFGTests-CNF0.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        cfg.toCNF(actual);

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[CFGTests] CFG2BNF") {
        CFG cfg("test/res/input/CFG2BNF.json");

        const std::ifstream expectedFile("test/res/expected/CFGTests-CFG2BNF.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        cfg.toBNF();

        cfg.print(actual);

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[CFGTests] BNF2CFG") {
        CFG cfg("test/res/input/BNF2CFG.json");

        const std::ifstream expectedFile("test/res/expected/CFGTests-BNF2CFG.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        cfg.print(actual);

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[CFGTests] EBNF2CFG") {
        const CFG cfg = CFG::parse_ebnf("test/res/input/EBNF2CFG.txt");

        const std::ifstream expectedFile("test/res/expected/CFGTests-EBNF2CFG.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        // std::ofstream reset("test/res/expected/CFGTests-EBNF2CFG.txt");

        cfg.print(actual);

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[CFGTests] CYK") {
        CFG cfg("test/res/input/CYK0.json");

        CHECK_FALSE(!cfg.accepts("baaba"));
        CHECK_FALSE(cfg.accepts("abba"));
    }

    TEST_CASE("[CFGTests] first_and_follow") {
        CFG cfg = CFG::parse_ebnf("tmp.txt");

        std::set<std::string> expected{"(", "<EOS>"};

        cfg.follow("<arguments>");
    }
}
