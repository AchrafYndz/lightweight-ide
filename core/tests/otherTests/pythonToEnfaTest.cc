#include <sstream>
#include "../../lib/doctest/doctest.h"

#include "../../lib/path/path.h"
#include "../../src/other/pythonToEnfa.h"

TEST_SUITE("pythonToEnfaTests") {
    TEST_CASE("[pythonToEnfaTests] scan0") {
        pythonToEnfa p;

        const std::ifstream expectedFile(path::rootDirectory +
                                         "/tests/otherTests/res/expected/otherTests-scan0exp.txt");

        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        p.scanToFile(actual, path::rootDirectory + "/tests/otherTests/res/input/scan0.txt");

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[pythonToEnfaTests] splitAndIdentify0") {
        pythonToEnfa p;

        const std::ifstream expectedFile(path::rootDirectory +
                                         "/tests/otherTests/res/expected/splitAndIdentify0exp.txt");

        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        p.generateAutomata(path::rootDirectory + "/tests/otherTests/res/input/pythonKeyw.txt");
        p.generateDfaFromEnfas(p.enfasKeyw);
        p.splitAndIdentify(path::rootDirectory + "/tests/otherTests/res/input/splitAndIdentify0.py");
        p.printIdentifiedTokens(actual);

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[pythonToEnfaTests] splitAndIdentify1") {
        pythonToEnfa p;

        const std::ifstream expectedFile(path::rootDirectory +
                                         "/tests/otherTests/res/expected/splitAndIdentify1exp.txt");

        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        p.generateAutomata(path::rootDirectory + "/tests/otherTests/res/input/pythonKeyw.txt");
        p.generateDfaFromEnfas(p.enfasKeyw);
        p.splitAndIdentify(path::rootDirectory + "/tests/otherTests/res/input/splitAndIdentify1.py");
        p.printIdentifiedTokens(actual);

        CHECK_EQ(expected.str(), actual.str());
    }
}