#ifndef IDE_RE_H
#define IDE_RE_H

#include "ENFA.h"

#include <queue>
#include <stack>
#include <string>
#include <vector>

class RE {
    std::string re;
    char eps;
    std::vector<char> alphabet;

  public:
    RE() = default;
    RE(std::string re, const char eps);

    ENFA toENFA() const;

    std::string toPostfixExp(const std::string &re) const;

    bool precedence(const char a, const char b) const;

    typedef std::map<std::string, std::map<char, std::vector<std::string>>> transitionTable;

    std::tuple<transitionTable, std::string, std::string> generateKleenENFA(
      const std::pair<std::string, std::pair<std::string, std::string>> &a, unsigned int &counter) const;
    std::tuple<transitionTable, std::string, std::string> generatePlusENFA(
      const std::pair<std::string, std::pair<std::string, std::string>> &a,
      const std::pair<std::string, std::pair<std::string, std::string>> &b, unsigned int &counter) const;
    std::tuple<transitionTable, std::string, std::string> generateConcatenationENFA(
      const std::pair<std::string, std::pair<std::string, std::string>> &a,
      const std::pair<std::string, std::pair<std::string, std::string>> &b, unsigned int &counter) const;

    const std::vector<char> &getAlphabet() const { return alphabet; }
    const std::string &getRE() const { return re; }
    char getEpsilon() const { return eps; }
};

#endif  // IDE_RE_H