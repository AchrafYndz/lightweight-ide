#include <sstream>
#include "../../lib/doctest/doctest.h"

#include "../../lib/path/path.h"
#include "../../src/automata/PDFA.h"

TEST_SUITE("PDFATests") {
    TEST_CASE("[PDFATests] parseFile0") {
        const PDFA pdfa(path::rootDirectory + "/tests/automataTests/res/input/PDFA0.json");

        // load in expected file
        const std::ifstream expectedFile(path::rootDirectory +
                                         "/tests/automataTests/res/expected/PDFATests-parseFile0.json");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        // std::ofstream tmp(path::rootDirectory + "/tests/automataTests/res/expected/PDFATests-parseFile0.json");

        pdfa.print(actual);

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[PDFATests] parseFile1") {
        const PDFA pdfa(path::rootDirectory + "/tests/automataTests/res/input/PDFA1.json");

        // load in expected file
        const std::ifstream expectedFile(path::rootDirectory +
                                         "/tests/automataTests/res/expected/PDFATests-parseFile1.json");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        // std::ofstream tmp(path::rootDirectory + "/tests/automataTests/res/expected/PDFATests-parseFile1.json");

        pdfa.print(actual);

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[PDFATests] inputString0") {
        PDFA pdfa(path::rootDirectory + "/tests/automataTests/res/input/PDFA0.json");

        pdfa.input("c");

        const std::string currentStateName = pdfa.getCurrentState()->name;

        const bool valid = currentStateName == "1" || currentStateName == "3" || currentStateName == "4";
        CHECK(valid);
    }

    TEST_CASE("[PDFATests] autocomplete0") {
        PDFA pdfa(path::rootDirectory + "/tests/automataTests/res/input/autocompletionPDFA0.json");

        pdfa.input("asse");

        const std::string &prediction = pdfa.predict();
        CHECK_EQ("assert", prediction);
    }

    TEST_CASE("[PDFATests] minimization0") {
        PDFA pdfa(path::rootDirectory + "/tests/automataTests/res/input/PDFA2.json");

        PDFA min = pdfa.minimize(0.10);

        // load in expected file
        const std::ifstream expectedFile(path::rootDirectory +
                                         "/tests/automataTests/res/expected/PDFATests-PDFA2.json");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

    //    std::ofstream tmp(path::rootDirectory + "/tests/automataTests/res/expected/PDFATests-PDFA2.json");

        pdfa.print(actual);

        CHECK_EQ(expected.str(), actual.str());
    }
}
