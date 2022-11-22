#include "doctest/doctest.h"
#include <fstream>
#include <sstream>

#include "automata/PDA.h"

TEST_SUITE("PDATests") {
    TEST_CASE("[PDATests] parseFile0") {
        PDA pda("test/res/input/PDA0.json");

        const std::ifstream expectedFile("test/res/expected/PDATests-parseFile0.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        pda.print(actual);

        CHECK_EQ(expected.str(), actual.str());
    }
    TEST_CASE("[PDATests] PDA2CFG") {
        PDA pda("test/res/input/PDA2CFG0.json");

        const std::ifstream expectedFile("test/res/expected/PDATests-PDA2CFG0.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        CFG cfg = pda.toCFG();
        cfg.print(actual);

        CHECK_EQ(expected.str(), actual.str());
    }
}