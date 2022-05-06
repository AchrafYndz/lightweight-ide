#ifndef IDE_DFA_H
#define IDE_DFA_H

#include "FA.h"

using TFA = std::vector<std::pair<std::string, std::vector<std::pair<std::string, bool>>>>;

class DFA : public FA {
  public:
    DFA() = default;
    DFA(const std::string &file) : FA(file){};
    DFA(const std::vector<char> alphabet, const std::vector<State *> states,
        const std::vector<Transition *> transitions);
};

#endif  // IDE_DFA_H
