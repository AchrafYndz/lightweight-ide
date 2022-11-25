#ifndef IDE_SRC_AUTOMATA_PDA_H
#define IDE_SRC_AUTOMATA_PDA_H

#include "nlohmann/json.hpp"

#include "../machines/CFG.h"
#include "PDAInput.h"

#include <fstream>
#include <map>
#include <set>

class PDA {
private:
    struct PDATransitionCompare {
        inline bool operator()(const PDAInput* leftKey, const PDAInput* rightKey) const {
            const std::string a = leftKey->getState() + leftKey->getInput() + leftKey->getStackTop();
            const std::string b = rightKey->getState() + rightKey->getInput() + rightKey->getStackTop();

            return a < b;
        }
    };

public:
    PDA(std::string fileName);

    CFG toCFG();

    void print(std::ostream& out = std::cout) const;

    const std::vector<std::string>& getStates() const;

    const std::vector<std::string>& getAlphabet() const;

    const std::vector<std::string>& getStackAlphabet() const;

    const std::map<PDAInput*, std::pair<std::string, std::vector<std::string>>, PDATransitionCompare>& getTransitions()
        const;

    const std::string& getStartState() const;

    const std::string& getStartStack() const;

private:
    std::vector<std::string> states;
    std::vector<std::string> alphabet;
    std::vector<std::string> stackAlphabet;
    std::map<PDAInput*, std::pair<std::string, std::vector<std::string>>, PDATransitionCompare> transitions;
    std::string startState;
    std::string startStack;
};

#endif // IDE_SRC_AUTOMATA_PDA_H
