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
    DFA(const DFA& dfa);
    DFA& operator=(const DFA& dfa);
    DFA minimize();
    void printTable();
    void printTable(TFA &t);

    void printStats(std::ostream &out) const;
    void printStats() const;

    DFA() = default;
    DFA(const std::string &file) : FA(file){};
    DFA(const std::vector<char> &alphabet, const std::vector<State *> &states,
        const std::vector<Transition *> &transitions);
    DFA(const DFA &dfa1, const DFA &dfa2, bool cross);
    DFA(std::vector<DFA> &dfas, bool cross);
    bool accepts(const std::string &str) const;

    bool operator==(DFA &rhs);

};

#endif  // IDE_DFA_H
