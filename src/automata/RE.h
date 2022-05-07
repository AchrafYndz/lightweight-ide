#ifndef IDE_RE_H
#define IDE_RE_H

#include "ENFA.h"

#include <string>
#include <vector>

class RE {
    std::string re;
    char eps;
    std::vector<char> alphabet;

  public:
    RE() = default;
    RE(const std::string &re, const char eps);

    ENFA toENFA() const;

    std::string toPostfixExp(const std::string &re) const;

    bool precedence(const char a, const char b) const;

    
};

#endif  // IDE_RE_H