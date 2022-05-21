#ifndef IDE_PYTHON_TO_ENFA_H
#define IDE_PYTHON_TO_ENFA_H

#include "../../src/automata/RE.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

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

    bool isKeyword(const std::string& str, const std::string& file) const;

    std::vector<char> getSigma() const;
    std::string expand(std::vector<char>& inputSet) const;
    std::string replaceRegexOp(const std::string& str) const;
    bool isComment(const std::string& str) const;
    std::string recognizeToken(const std::string& str) const;

};

#endif  // IDE_PYTHON_TO_ENFA_H
