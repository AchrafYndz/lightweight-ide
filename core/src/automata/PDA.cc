#include "PDA.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

#include "Value.h"

static void all_combinations(std::vector<std::string> input, std::vector<std::vector<std::string>>& result) {
    std::vector<std::vector<std::string>> res;
    do {
        res.emplace_back(input);
    } while (std::next_permutation(input.begin(), input.end()));
    result = res;
}

PDA::PDA(std::string fileName) {
    std::ifstream input(fileName);
    nlohmann::json j;
    input >> j;

    std::vector<std::string> states;
    for (std::string stateName : j["States"]) {
        states.emplace_back(stateName);
    }
    PDA::states = states;

    std::vector<std::string> alphabet;
    for (std::string charName : j["Alphabet"]) {
        alphabet.emplace_back(charName);
    }
    PDA::alphabet = alphabet;

    std::vector<std::string> stackAlphabet;
    for (std::string charName : j["StackAlphabet"]) {
        stackAlphabet.emplace_back(charName);
    }
    PDA::stackAlphabet = stackAlphabet;

    std::map<PDAInput*, std::pair<std::string, std::vector<std::string>>> transitions = {};
    for (nlohmann::json tValue : j["Transitions"]) {
        std::string fromState = tValue["from"];
        std::string input = tValue["input"];
        std::string stackTop = tValue["stacktop"];
        std::string toState = tValue["to"];
        std::vector<std::string> replacement = tValue["replacement"];
        PDAInput* pdaInput = new PDAInput(fromState, input, stackTop);
        transitions.insert({pdaInput, {toState, replacement}});
    }
    PDA::transitions = transitions;

    PDA::startState = j["StartState"];
    PDA::startStack = j["StartStack"];
}

const std::vector<std::string>& PDA::getStates() const { return states; }

const std::vector<std::string>& PDA::getAlphabet() const { return alphabet; }

const std::vector<std::string>& PDA::getStackAlphabet() const { return stackAlphabet; }

const std::map<PDAInput*, std::pair<std::string, std::vector<std::string>>>& PDA::getTransitions() const {
    return transitions;
}

const std::string& PDA::getStartState() const { return startState; }

const std::string& PDA::getStartStack() const { return startStack; }

CFG PDA::toCFG() {
    // start by setting s productions
    CFG* cfg = new CFG();

    Value* epsilon = new Value(" ", true);

    // new V, T, P of CFG
    Values V;
    Values T;
    std::map<std::string, std::vector<std::vector<Value*>>> P;

    T.insert({" ", epsilon});

    // Start Variable
    Value* S = new Value("S", false);
    V.insert({"S", S});
    cfg->setStart(S);

    // Make all new possible variables
    for (auto fstate : PDA::states) {
        for (auto stackSymbol : PDA::stackAlphabet) {
            for (auto sstate : PDA::states) {
                Value* newV = new Value("[" + fstate + "," + stackSymbol + "," + sstate + "]", false);
                V.insert({newV->getName(), newV});
            }
        }
    }
    cfg->setVariables(V);

    // Make all new terminals
    for (auto terminal : PDA::alphabet) {
        Value* newT = new Value(terminal, true);
        T.insert({newT->getName(), newT});
    }
    cfg->setTerminals(T);

    // rules
    // rule 1 S -> [q0, X, rk]
    std::vector<std::vector<Value*>> startvarProductions;
    for (auto state : PDA::states) {
        //        std::vector<Value*> startProds = {cfg->makeValue("[" + PDA::startState + "," + startStack + "," +
        //        state + "]")};
        std::vector<Value*> startProds = {
            new Value{"[" + PDA::startState + "," + startStack + "," + state + "]", false}};
        startvarProductions.emplace_back(startProds);
    }
    P.insert({"S", startvarProductions});
    // rule 4 d(q, ε, X) = (r, ε)
    // => [q,X,r] = ε
    for (auto transition : PDA::transitions) {
        PDAInput* input = &(*transition.first);                                      // d(q, a, X)
        std::pair<std::string, std::vector<std::string>> result = transition.second; // (r, Y1Y2...Yk)
        std::string leftState = input->getState();
        std::string terminal = input->getInput();
        std::string stackTop = input->getStackTop();
        std::string toState = result.first;
        std::vector<std::string> replacement = result.second;
        if (replacement.size() != 0 || terminal.length() != 0)
            continue;
        std::string stateName = "[" + leftState + "," + stackTop + "," + toState + "]";
        //        std::vector<Value*> varProds = {cfg->getVar(" ")};
        std::vector<Value*> varProds = {new Value{" ", false}};
        P[stateName].emplace_back(varProds);
    }
    // rule 2: d(q, a, X) = (r, Y1Y2...Yk)
    // => [q,X,rk] = a[r,Y1,r1][r1,Y2,r2]....[rk-1,Yk,rk]
    //     d(q, a, X) = (r, X)
    // => [q,X,rk] = a[r,X,rk]
    for (auto transition : PDA::transitions) {
        PDAInput* input = transition.first;                                          // d(q, a, X)
        std::pair<std::string, std::vector<std::string>> result = transition.second; // (r, Y1Y2...Yk)
        std::string leftState = input->getState();
        std::string terminal = input->getInput();
        std::string stackTop = input->getStackTop();
        std::string toState = result.first;
        std::vector<std::string> replacement = result.second;
        if (terminal == "")
            continue;
        if (replacement.size() == 0)
            continue;
        std::vector<std::vector<std::string>> all_possible_orders_state;
        all_combinations(PDA::states, all_possible_orders_state);
        for (auto state_group : all_possible_orders_state) { // all possibilities
            for (unsigned long i = 0; i < state_group.size(); i++) {
                std::string state_rk = state_group[i];
                std::string stateName = "[" + leftState + "," + stackTop + "," + state_rk + "]";
                //                std::vector<Value*> varProds = {cfg->getVar(terminal)};
                std::vector<Value*> varProds = {new Value{terminal, true}};
                for (unsigned long j = 0; j < state_group.size(); j++) {
                    if (replacement.size() == 1) {
                        //                        varProds.emplace_back(cfg->getVar("[" + toState + "," + replacement[0]
                        //                        + "," + state_rk + "]"));
                        varProds.emplace_back(
                            new Value{"[" + toState + "," + replacement[0] + "," + state_rk + "]", false});
                        i++;
                        break;
                    } else {
                        if (j == 0) {
                            //                            varProds.emplace_back(cfg->getVar("[" + toState + "," +
                            //                            replacement[j] + "," + state_group[j] + "]"));
                            varProds.emplace_back(
                                new Value{"[" + toState + "," + replacement[j] + "," + state_group[j] + "]", false});
                        } else {
                            if (state_group[j] == state_rk) {
                                //                                varProds.emplace_back(cfg->getVar("[" + state_group[j
                                //                                - 1] + "," + replacement[j] +"," + state_group[j] +
                                //                                "]"));
                                varProds.emplace_back(new Value{"[" + state_group[j - 1] + "," + replacement[j] + "," +
                                                                    state_group[j] + "]",
                                                                false});
                            } else {
                                //                                varProds.emplace_back(cfg->getVar("[" + state_group[j
                                //                                - 1] + "," + replacement[j] +"," + state_rk + "]"));
                                varProds.emplace_back(new Value{
                                    "[" + state_group[j - 1] + "," + replacement[j] + "," + state_rk + "]", false});
                            }
                        }
                    }
                }
                P[stateName].emplace_back(varProds);
            }
        }
    }
    // rule 3: d(q, a, X) = (r, ε)
    // => [q,X,r] = a
    for (auto transition : PDA::transitions) {
        PDAInput* input = transition.first;                                          // d(q, a, X)
        std::pair<std::string, std::vector<std::string>> result = transition.second; // (r, Y1Y2...Yk)
        std::string leftState = input->getState();
        std::string terminal = input->getInput();
        std::string stackTop = input->getStackTop();
        std::string toState = result.first;
        std::vector<std::string> replacement = result.second;
        if (replacement.size() != 0 || terminal == "")
            continue;
        std::string stateName = "[" + leftState + "," + stackTop + "," + toState + "]";
        //        std::vector<Value*> varProds = {cfg->getVar(terminal)};
        std::vector<Value*> varProds = {new Value{terminal, true}};
        P[stateName].emplace_back(varProds);
    }
    cfg->setProductions(P);
    return *cfg;
}

void PDA::print(std::ostream& out) const {
    out << "States:" << std::endl;
    for (std::string state : states) {
        out << "\t" << state << "\n";
    }
    out << "Alphabet:" << std::endl;
    for (std::string letter : alphabet) {
        out << "\t" << letter << "\n";
    }
    out << "Stack Alphabet:" << std::endl;
    for (std::string stackLetter : stackAlphabet) {
        out << "\t" << stackLetter << "\n";
    }
    out << "Transitions:" << std::endl;
    for (auto transition : transitions) {
        out << "\tfrom \'" << transition.first->getState() << "\' with input \'" << transition.first->getInput()
            << "\' and stacktop \'" << transition.first->getStackTop() << "\', to \'" << transition.second.first << "\' replacing ";
        for (std::string right: transition.second.second) {
            out << right << " ";
        }
        out << "\n";
    }
    out << "Start state: \n" << startState << std::endl;
    out << "Start stacksymbol: \n" << startStack << std::endl;
}