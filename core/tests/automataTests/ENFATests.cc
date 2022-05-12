#include "../../lib/doctest/doctest.h"

#include "../../lib/path/path.h"
#include "../../src/automata/ENFA.h"

#include <fstream>
#include <sstream>

TEST_SUITE("ENFATests") {
    TEST_CASE("[ENFATests] parseFile0") {
        const ENFA enfa(path::rootDirectory + "/tests/automataTests/res/input/ENFA0.json");

        // load in expected file
        const std::ifstream expectedFile(path::rootDirectory +
                                         "/tests/automataTests/res/expected/ENFATests-parseFile0.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        // std::ofstream tmp(path::rootDirectory + "/tests/automataTests/res/expected/ENFATests-parseFile0.txt");

        enfa.printStats(actual);

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[ENFATests] parseFile1") {
        const ENFA enfa(path::rootDirectory + "/tests/automataTests/res/input/ENFA1.json");

        // load in expected file
        const std::ifstream expectedFile(path::rootDirectory +
                                         "/tests/automataTests/res/expected/ENFATests-parseFile1.txt");
        std::stringstream expected;
        expected << expectedFile.rdbuf();

        std::stringstream actual("");

        // std::ofstream tmp(path::rootDirectory + "/tests/automataTests/res/expected/ENFATests-parseFile1.txt");

        enfa.printStats(actual);

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[ENFATests] accepts0") {
        ENFA enfa;

        enfa.setAlphabet({'c'});
        enfa.setEpsilon('e');

        std::vector<State *> states;
        states.push_back(new State{.name = "0", .starting = true, .accepting = false});
        states.push_back(new State{.name = "1", .starting = false, .accepting = true});
        states.push_back(new State{.name = "2", .starting = false, .accepting = false});
        states.push_back(new State{.name = "3", .starting = false, .accepting = false});
        states.push_back(new State{.name = "4", .starting = false, .accepting = false});
        enfa.setStates(states);

        std::vector<Transition *> transitions;
        transitions.push_back(new Transition{.from = enfa.findState("0"), .to = enfa.findState("3"), .input = 'c'});
        transitions.push_back(new Transition{.from = enfa.findState("1"), .to = enfa.findState("4"), .input = 'c'});
        transitions.push_back(new Transition{.from = enfa.findState("2"), .to = enfa.findState("1"), .input = 'c'});
        transitions.push_back(new Transition{.from = enfa.findState("2"), .to = enfa.findState("1"), .input = 'e'});
        transitions.push_back(new Transition{.from = enfa.findState("3"), .to = enfa.findState("2"), .input = 'c'});
        transitions.push_back(new Transition{.from = enfa.findState("4"), .to = enfa.findState("2"), .input = 'c'});
        enfa.setTransitions(transitions);

        SUBCASE("[ENFATests]-accepts expectTrue") {
            CHECK(enfa.accepts("cc"));
            CHECK(enfa.accepts("ccc"));
            CHECK(enfa.accepts("cccc"));
            CHECK(enfa.accepts("ccccc"));
            CHECK(enfa.accepts("cccccc"));
            CHECK(enfa.accepts("ccccccc"));
        }

        SUBCASE("[ENFATests]-accepts expectFalse") {
            CHECK_FALSE(enfa.accepts(""));
            CHECK_FALSE(enfa.accepts("c"));
        }
    }

    TEST_CASE("[ENFATests] accepts1") {
        ENFA enfa;

        enfa.setAlphabet({'b', 'c'});
        enfa.setEpsilon('e');

        std::vector<State *> states;
        states.push_back(new State{.name = "0", .starting = true, .accepting = false});
        states.push_back(new State{.name = "1", .starting = false, .accepting = false});
        states.push_back(new State{.name = "2", .starting = false, .accepting = false});
        states.push_back(new State{.name = "3", .starting = false, .accepting = false});
        states.push_back(new State{.name = "4", .starting = false, .accepting = true});
        enfa.setStates(states);

        std::vector<Transition *> transitions;
        transitions.push_back(new Transition{.from = enfa.findState("0"), .to = enfa.findState("1"), .input = 'e'});
        transitions.push_back(new Transition{.from = enfa.findState("1"), .to = enfa.findState("2"), .input = 'e'});
        transitions.push_back(new Transition{.from = enfa.findState("2"), .to = enfa.findState("3"), .input = 'e'});
        transitions.push_back(new Transition{.from = enfa.findState("3"), .to = enfa.findState("4"), .input = 'b'});
        enfa.setTransitions(transitions);

        SUBCASE("[ENFATests]-accepts expectTrue") { CHECK(enfa.accepts("b")); }

        SUBCASE("[ENFATests]-accepts expectFalse") {
            CHECK_FALSE(enfa.accepts(""));
            CHECK_FALSE(enfa.accepts("bb"));
            CHECK_FALSE(enfa.accepts("c"));
            CHECK_FALSE(enfa.accepts("cc"));
        }
    }
}