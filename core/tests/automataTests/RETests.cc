#include <sstream>
#include "../../lib/doctest/doctest.h"

#include "../../lib/path/path.h"
#include "../../src/automata/RE.h"

TEST_SUITE("RETests") {
    TEST_CASE("[RETests] parseString0") {
        const RE re("e+y+m+i", 'e');

        const std::set<char> expectedAlphabet = {'y', 'm', 'i'};
        const char expectedEps = 'e';

        const std::vector<char> tmp = re.getAlphabet();
        const std::set<char> actualAlphabet = std::set<char>(tmp.begin(), tmp.end());
        const char actualEps = re.getEpsilon();

        CHECK_EQ(expectedAlphabet, actualAlphabet);
        CHECK_EQ(expectedEps, actualEps);
    }

    TEST_CASE("[RETests] parseString1") {
        const RE re("(e + i)(e + y)*(e + (m + i)*)", 'e');

        const std::set<char> expectedAlphabet = {'y', 'm', 'i'};
        const char expectedEps = 'e';

        const std::vector<char> tmp = re.getAlphabet();
        const std::set<char> actualAlphabet = std::set<char>(tmp.begin(), tmp.end());
        const char actualEps = re.getEpsilon();

        CHECK_EQ(expectedAlphabet, actualAlphabet);
        CHECK_EQ(expectedEps, actualEps);
    }

    TEST_CASE("[RETests] toENFA0") {
        const RE re("e+y+m+i", 'e');
        const ENFA enfa = re.toENFA();

        // load in expected file
        const std::ifstream expectedFile(path::rootDirectory + "/tests/automataTests/res/expected/RETests-toENFA0.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        // std::ofstream tmp(path::rootDirectory + "/tests/automataTests/res/expected/RETests-toENFA0.txt");

        enfa.printStats(actual);

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[RETests] toENFA1") {
        const RE re("(e + i)(e + y)*(e + (m + i)*)", 'e');
        const ENFA enfa = re.toENFA();

        // load in expected file
        const std::ifstream expectedFile(path::rootDirectory + "/tests/automataTests/res/expected/RETests-toENFA1.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        // std::ofstream tmp(path::rootDirectory + "/tests/automataTests/res/expected/RETests-toENFA1.txt");

        enfa.printStats(actual);

        CHECK_EQ(expected.str(), actual.str());
    }
}