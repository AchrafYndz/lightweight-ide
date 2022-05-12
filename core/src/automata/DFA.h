#ifndef IDE_DFA_H
#define IDE_DFA_H

#include "FA.h"

using TFA = std::vector<std::pair<std::string, std::vector<std::pair<std::string, bool>>>>;

class DFA : public FA {
  private:  // table filling algorithm
    TFA generateTFA();
    static bool pairCrossed(TFA &t, const std::string &s1, const std::string &s2);
    std::set<std::set<State *>> findEquivalenceClasses(TFA &t) const;
    std::set<State *> findEquivalentStates(TFA &t, State *s) const;

  public:  // table filling algorithm
    DFA minimize();
    void printTable();
    void printTable(TFA &t);

    DFA() = default;
    DFA(const std::string &file) : FA(file){};
    DFA(const std::vector<char> &alphabet, const std::vector<State *> &states,
        const std::vector<Transition *> &transitions);

    bool operator==(DFA &rhs);
};

#endif  // IDE_DFA_H
