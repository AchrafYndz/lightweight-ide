#ifndef IDE_PYTHON_TO_ENFA_H
#define IDE_PYTHON_TO_ENFA_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "../../src/automata/RE.h"

typedef std::pair<int, int> pos;

class pythonToEnfa {
  public:
    // reads python keywords from file and places them in vector
    std::vector<std::string> getPythonKeyw(const std::string &file) const;

    // reads all characters from file, store them in vector with row and column number
    std::vector<std::pair<char, std::pair<int, int>>> scan(const std::string &file) const;
    void scanToFile(std::ostream &out, const std::string &file) const;

    // splits text in potential tokens
    std::map<std::pair<std::pair<int, int>, std::pair<int, int>>, std::string> splitText(const std::string &file) const;

    void printMap(std::map<std::pair<std::pair<int, int>, std::pair<int, int>>, std::string> &m,
                  std::ostream &out) const;

    std::vector<char> getSigma() const;
    void removeElementsFromSet(std::vector<char> &inputSet, char ch) const;
    std::string expand(std::vector<char> &inputSet) const;
    std::string replaceRegexOp(const std::string &str) const;

    std::vector<ENFA> generateEnfaKeywords(const std::string& file) const;
    std::vector<ENFA> generateEnfaComments() const;
    std::vector<ENFA> generateEnfaStrings() const;
    void generateDfaFromEnfas(const std::vector<ENFA>& enfas);
    void generateAutomata(const std::string& file);

    bool isKeyword(const std::string& str, std::vector<ENFA>& enfaKeyw) const;
    bool isComment(const std::string& str, std::vector<ENFA>& enfaComments) const;
    bool isString(const std::string& str, std::vector<ENFA>& enfaStrings) const;

    void identifyTokens(const std::map<std::pair<std::pair<int, int>, std::pair<int, int>>, std::string> &m);
    void splitAndIdentify(const std::string &file);
    void outputTokesAsJson(nlohmann::json &out) const;
    void printIdentifiedTokens(std::ostream &out) const;
    std::vector<std::pair<pos, pos>> keywords;
    std::vector<std::pair<pos, pos>> comments;
    std::vector<std::pair<pos, pos>> strings;

    DFA dfaKeyw;
    std::vector<ENFA> enfasKeyw;
    std::vector<ENFA> enfasComments;
    std::vector<ENFA> enfasStrings;
};

#endif  // IDE_PYTHON_TO_ENFA_H
