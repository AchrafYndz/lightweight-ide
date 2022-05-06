#ifndef IDE_NFA_H
#define IDE_NFA_H

#include "FA.h"

class NFA : public FA {
  public:
    NFA() = default;
    NFA(const std::string &file) : FA(file) {}
    NFA(const std::vector<char> alphabet, const std::vector<State *> states,
        const std::vector<Transition *> transitions);
};

#endif  // IDE_NFA_H
