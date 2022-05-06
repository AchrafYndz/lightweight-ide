#include "DFA.h"

DFA::DFA(const std::vector<char> alphabet, const std::vector<State *> states,
         const std::vector<Transition *> transitions) :
    FA(alphabet, states, transitions) {}