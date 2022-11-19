#include <fstream>
#include <sstream>
#include "../../lib/doctest/doctest.h"

#include "../../lib/path/path.h"
#include "../../src/machines/CFG.H"

TEST_SUITE("CFGTests") {
    TEST_CASE("[CFGTests] parseFile0") {
        CFG cfg(path::rootDirectory + "/tests/machinesTests/res/input/CFG0.json");

        const std::ifstream expectedFile(path::rootDirectory +
                                         "/tests/machinesTests/res/expected/CFGTests-parseFile0.json");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

//         std::ofstream tmp(path::rootDirectory + "/tests/automataTests/res/expected/DFATests-parseFile0.json");

        cfg.print(actual);

        CHECK_EQ(expected.str(), actual.str());
    }
}