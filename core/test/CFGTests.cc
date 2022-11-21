#include <fstream>
#include <sstream>
#include "doctest/doctest.h"

#include "../../src/machines/CFG.h"

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
}