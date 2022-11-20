#include <fstream>
#include <sstream>
#include "doctest/doctest.h"
#include "path/path.h"

#include "../../src/machines/CFG.h"

TEST_SUITE("CFGTests") {
    TEST_CASE("[CFGTests] parseFile0") {
        CFG cfg(path::rootDirectory + "/tests/machinesTests/res/input/CFG0.json");

        const std::ifstream expectedFile(path::rootDirectory +
                                         "/tests/machinesTests/res/expected/CFGTests-parseFile0.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        //    std::ofstream tmp(path::rootDirectory + "/tests/machinesTests/res/expected/CFGTests-parseFile0.txt");

        cfg.print(actual);

        CHECK_EQ(expected.str(), actual.str());
    }
}