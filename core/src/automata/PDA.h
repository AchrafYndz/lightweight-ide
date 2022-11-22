#include <set>
#include <map>
#include "PDAInput.h"
#include "nlohmann/json.hpp"
#include "machines/CFG.h"
#include <fstream>

#ifndef MB_ING_OEF1_PDA_H
#define MB_ING_OEF1_PDA_H


class PDA {
    std::vector<std::string> states;
    std::vector<std::string> alphabet;
    std::vector<std::string> stackAlphabet;
    std::map<PDAInput*, std::pair<std::string, std::vector<std::string>>> transitions;
    std::string startState;
    std::string startStack;
public:
    PDA(std::string fileName);

    CFG toCFG();

    void print(std::ostream& out = std::cout) const;

    const std::vector<std::string> &getStates() const;

    const std::vector<std::string> &getAlphabet() const;

    const std::vector<std::string> &getStackAlphabet() const;

    const std::map<PDAInput*, std::pair<std::string, std::vector<std::string>>> &getTransitions() const;

    const std::string &getStartState() const;

    const std::string &getStartStack() const;
};


#endif //MB_ING_OEF1_PDA_H
