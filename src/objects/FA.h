#ifndef IDE_FA_H
#define IDE_FA_H

#include "../../lib/nlohman-json/json.hpp"
#include "Components.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <vector>

class FA {
  protected:
    std::vector<char> alphabet;
    std::vector<State *> states;
    std::vector<Transition *> transitions;

  public:
    FA() = default;
    FA(const std::string &file);
    FA(const std::vector<char> alphabet, const std::vector<State *> states,
       const std::vector<Transition *> transitions);
    virtual ~FA();

    static std::string stateSetToName(const std::set<State *> &s);

    State *findState(const std::string &n) const;
    Transition *findTransition(const State *from, const char input) const;
    State *findStartingState() const;

    void print(std::ostream &out) const;
    void print() const;

    void setStates(std::vector<State *> &states);
    void setAlphabet(std::vector<char> &states);
    void setTransitions(std::vector<Transition *> &states);
    void renameState(const std::string &s, const std::string &n);
    void removeUnused();
};

#endif  // IDE_FA_H