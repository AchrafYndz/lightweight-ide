#include <fstream>
#include <sstream>
#include "../../lib/doctest/doctest.h"

#include "../../src/other/pythonToEnfa.h"
#include "../../lib/path/path.h"

TEST_SUITE("pythonToEnfaTests") {
    TEST_CASE("[pythonToEnfaTests] scan0") {
        pythonToEnfa p;

        const std::ifstream expectedFile(path::rootDirectory +
                                         "/tests/otherTests/res/expected/otherTests-scan0exp.txt");

        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        // std::ofstream tmp(path::rootDirectory + "/tests/automataTests/res/expected/DFATests-parseFile0.json");

        p.scanToFile(actual, path::rootDirectory + "/tests/otherTests/res/input/scan0.txt");

        CHECK_EQ(expected.str(), actual.str());
    }
}