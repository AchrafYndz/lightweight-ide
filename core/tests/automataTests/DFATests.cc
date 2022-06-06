#include <fstream>
#include <sstream>
#include "../../lib/doctest/doctest.h"

#include "../../lib/path/path.h"
#include "../../src/automata/DFA.h"

TEST_SUITE("DFATests") {
    TEST_CASE("[DFATests] parseFile0") {
        const DFA dfa(path::rootDirectory + "/tests/automataTests/res/input/DFA0.json");

        const std::ifstream expectedFile(path::rootDirectory +
                                         "/tests/automataTests/res/expected/DFATests-parseFile0.json");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        // std::ofstream tmp(path::rootDirectory + "/tests/automataTests/res/expected/DFATests-parseFile0.json");

        dfa.print(actual);

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[DFATests] parseFile1") {
        const DFA dfa(path::rootDirectory + "/tests/automataTests/res/input/DFA1.json");

        const std::ifstream expectedFile(path::rootDirectory +
                                         "/tests/automataTests/res/expected/DFATests-parseFile1.json");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        // std::ofstream tmp(path::rootDirectory + "/tests/automataTests/res/expected/DFATests-parseFile1.json");

        dfa.print(actual);

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[DFATests] productDFAUnion0") {
        const DFA dfa0(path::rootDirectory + "/tests/automataTests/res/input/DFA0.json");
        const DFA dfa1(path::rootDirectory + "/tests/automataTests/res/input/DFA1.json");

        const DFA product(dfa0, dfa1, false);

        const std::ifstream expectedFile(path::rootDirectory +
                                         "/tests/automataTests/res/expected/DFATests-productDFAUnion0.json");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        // std::ofstream tmp(path::rootDirectory + "/tests/automataTests/res/expected/DFATests-productDFAUnion0.json");

        product.print(actual);

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[DFATests] productDFAUnion1") {
        const DFA dfa0(path::rootDirectory + "/tests/automataTests/res/input/DFA0.json");
        const DFA dfa1(path::rootDirectory + "/tests/automataTests/res/input/DFA1.json");

        const DFA product(dfa0, dfa1, true);

        const std::ifstream expectedFile(path::rootDirectory +
                                         "/tests/automataTests/res/expected/DFATests-productDFAUnion1.json");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        // std::ofstream tmp(path::rootDirectory + "/tests/automataTests/res/expected/DFATests-productDFAUnion1.json");

        product.print(actual);

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[DFATests] accepts0") {
        const DFA dfa0(path::rootDirectory + "/tests/automataTests/res/input/DFA0.json");

        SUBCASE("[ENFATests]-accepts expectTrue") { CHECK(dfa0.accepts("xx")); }

        SUBCASE("[ENFATests]-accepts expectFalse") {
            CHECK_FALSE(dfa0.accepts(""));
            CHECK_FALSE(dfa0.accepts("x"));
            CHECK_FALSE(dfa0.accepts("xxx"));
            CHECK_FALSE(dfa0.accepts("xxxxx"));
        }
    }

    TEST_CASE("[DFATests] accepts0") {
        const DFA dfa0(path::rootDirectory + "/tests/automataTests/res/input/DFA1.json");

        SUBCASE("[ENFATests]-accepts expectTrue") {
            CHECK(dfa0.accepts("xx"));
            CHECK(dfa0.accepts("xxxxx"));
            CHECK(dfa0.accepts("xxxxxxxx"));
        }

        SUBCASE("[ENFATests]-accepts expectFalse") {
            CHECK_FALSE(dfa0.accepts(""));
            CHECK_FALSE(dfa0.accepts("x"));
            CHECK_FALSE(dfa0.accepts("xxx"));
            CHECK_FALSE(dfa0.accepts("xxxx"));
            CHECK_FALSE(dfa0.accepts("xxxxxx"));
        }
    }
}