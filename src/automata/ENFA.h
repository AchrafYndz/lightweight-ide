#ifndef IDE_ENFA_H
#define IDE_ENFA_H

#include "Components.h"
#include "FA.h"

#include <cassert>

class ENFA : public FA {
    char eps;

  public:
    ENFA() = default;
    ENFA(const std::string &file);
    ENFA(const std::vector<char> &alphabet, const std::vector<State *> &states,
         const std::vector<Transition *> &transitions, const char eps);

    ~ENFA() override = default;

    bool accepts(const std::string &str) const;

    std::vector<State *> closure(State *state) const;

    void printStats(std::ostream &out) const;
    void printStats() const;
};

#endif  // IDE_ENFA_H
