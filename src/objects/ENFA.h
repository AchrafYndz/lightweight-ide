#ifndef IDE_ENFA_H
#define IDE_ENFA_H

#include "FA.h"

class ENFA : public FA {
  private:
    char eps;

  public:
    ENFA() = default;
    ENFA(const std::string &file) : FA(file) {}
    ENFA(const std::vector<char> &alphabet, const std::vector<State *> &states,
         const std::vector<Transition *> &transitions, const char eps);
};

#endif  // IDE_ENFA_H
