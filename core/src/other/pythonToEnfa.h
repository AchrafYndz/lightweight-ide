#ifndef IDE_PYTHON_TO_ENFA_H
#define IDE_PYTHON_TO_ENFA_H

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
};

#endif  // IDE_PYTHON_TO_ENFA_H
