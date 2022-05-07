#include "ENFA.h"

ENFA::ENFA(const std::vector<char> &alphabet, const std::vector<State *> &states,
           const std::vector<Transition *> &transitions, const char eps) :
    FA(alphabet, states, transitions), eps(eps) {}