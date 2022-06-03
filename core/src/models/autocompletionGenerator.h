#ifndef IDE_AUTOCOMPLETIONGEREATION_H
#define IDE_AUTOCOMPLETIONGEREATION_H

#include "../automata/PDFA.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace models {
    PDFA genAutocompletionPDFA(const std::vector<std::string> &keywords,
                               const std::unordered_map<std::string, unsigned int> &frequencies);

    void genAutocompletionPDFAToFile(const std::vector<std::string> &keywords,
                                     const std::unordered_map<std::string, unsigned int> &frequencies,
                                     std::ostream &out);
}  // namespace models

#endif  // IDE_AUTOCOMPLETIONGEREATION_H