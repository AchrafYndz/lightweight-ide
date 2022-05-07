#include <fstream>
#include <sstream>
#include "../../lib/doctest/doctest.h"

#include "../../lib/path/path.h"
#include "../../src/automata/ENFA.h"

TEST_SUITE("ENFATests") {
    TEST_CASE("[ENFATests] parseFile") {
        const ENFA enfa(path::rootDirectory + "/tests/automataTests/res/input/ENFA0.json");

        // load in expected file
        const std::ifstream expectedFile(path::rootDirectory +
                                         "/tests/automataTests/res/expected/ENFATests-parseFile.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        // std::ofstream tmp(path::rootDirectory + "/tests/automataTests/res/expected/ENFATests-parseFile.txt");

        enfa.printStats(actual);

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[ENFATests] accepts0") {
        const ENFA enfa(path::rootDirectory + "/tests/automataTests/res/input/ENFA0.json");

        SUBCASE("[ENFATests]-accepts0 expectTrue") {
            CHECK(enfa.accepts("cc"));
            CHECK(enfa.accepts("ccc"));
            CHECK(enfa.accepts("cccc"));
            CHECK(enfa.accepts("ccccc"));
            CHECK(enfa.accepts("cccccc"));
            CHECK(enfa.accepts("ccccccc"));
        }
        SUBCASE("[ENFATests]-accepts0 expectFalse") {
            CHECK_FALSE(enfa.accepts(""));
            CHECK_FALSE(enfa.accepts("c"));
        }
    }
}