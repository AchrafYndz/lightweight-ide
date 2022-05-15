#include <fstream>
#include <sstream>
#include "../../lib/doctest/doctest.h"

#include "../../src/other/python_to_enfa.h"
#include "../../lib/path/path.h"

TEST_SUITE("python_to_enfaTests") {
    TEST_CASE("[python_to_enfaTests] scan0") {
        python_to_enfa p;

        const std::ifstream expectedFile(path::rootDirectory +
                                         "/tests/otherTests/res/expected/otherTests-scan0exp.txt");

        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        // std::ofstream tmp(path::rootDirectory + "/tests/automataTests/res/expected/DFATests-parseFile0.json");

        p.scan_to_file(actual, path::rootDirectory + "/tests/otherTests/res/input/scan0.txt");

        CHECK_EQ(expected.str(), actual.str());
    }
}