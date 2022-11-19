#include <iostream>
#include <iomanip>
#include <fstream>
#include <regex>
#include "../../../../../Downloads/CFG-main/json.hpp"
#include "CFG.h"

using json = nlohmann::json;

CFG::CFG() {
    variables = {"BINDIGIT", "S"};
    terminals = {"0", "1", "a", "b"};
    productions = {
            {"BINDIGIT", {"0", "1"}},
            {"S",        {"",  "a S b BINDIGIT"}}
    };
    startSymbol = "S";
}

void CFG::print(std::ostream &out) {
    // print variables
    out << "V = {";
    for (auto it = variables.begin(); it != variables.end(); ++it) {
        if (it != variables.begin())
            out << ", ";
        out << *it;
    }
    out << "}" << std::endl;

    // print terminals
    out << "T = {";
    for (auto it = terminals.begin(); it != terminals.end(); ++it) {
        if (it != terminals.begin())
            out << ", ";
        out << *it;
    }
    out << "}" << std::endl;

    // print productions
    std::vector<std::string> sortedRules;
    out << "P = {" << std::endl;
    for (auto &production: productions) {
        sortedRules.clear();
        for (const std::string &value: production.second) {
            sortedRules.push_back("`" + value + "`");
        }
        sort(sortedRules.begin(), sortedRules.end());
        for (const std::string &value: sortedRules) {
            out << "  " << production.first << " -> " << value << std::endl;
        }
    }
    out << "}" << std::endl;

    // print start symbol
    out << "S = " << startSymbol << std::endl;
}

CFG::CFG(const std::string &filename) {
    std::ifstream input(filename);

    nlohmann::json j;
    input >> j;

    // Assign the productions
    for (auto p: j["Productions"]) {
        std::set<std::string> body = {};
        std::string str;
        for (const auto &b: p["body"]) {
            str += std::string(b) + " ";
        }
        if (!str.empty()) {
            str.resize(str.size() - 1);
        }
        if (productions.find(std::string(p["head"])) == productions.end()) {
            body.insert(str);
            productions.insert({std::string(p["head"]), body});
        } else {
            productions[std::string(p["head"])].insert(str);
        }

    }

    // Assign the start symbol
    startSymbol = j["Start"];

    // Assign the terminals
    for (const auto &t: j["Terminals"]) {
        terminals.insert(std::string(t));
    }

    // Assign the variables
    for (const auto &v: j["Variables"]) {
        variables.insert(std::string(v));
    }
}

void CFG::toCNF() {
    std::cout << "Original CFG:" << "\n" << std::endl;
    print(std::cout);
    std::cout << "\n-------------------------------------\n" << std::endl;
    int originalCount = countProductions();
    // 1. Eliminate all epsilon productions
    std::cout << " >> Eliminating epsilon productions" << std::endl;
    std::set<std::string> nullables;
    for (auto &production: productions) {
        if (std::find(production.second.begin(), production.second.end(), "") != production.second.end()) {
            production.second.erase("");
            nullables.insert(production.first);
        }
    }

    std::set<std::string> recentlyAdded = nullables;
    while (!recentlyAdded.empty()) {
        std::set<std::string> lastlyAdded = recentlyAdded;
        recentlyAdded.clear();
        for (const std::string &nullable: lastlyAdded) {
            for (auto production: productions) {
                if (std::find(production.second.begin(), production.second.end(), nullable) !=
                    production.second.end()) {
                    nullables.insert(production.first);
                    recentlyAdded.insert(production.first);
                }
            }
        }
    }

    std::cout << "  Nullables are {";
    for (auto it = nullables.begin(); it != nullables.end(); it++) {
        if (it != nullables.begin()) std::cout << ", ";
        std::cout << *it;
    }
    std::cout << "}" << std::endl;


    bool madeChange = true;
    while (madeChange) {
        madeChange = false;
        for (const std::string &nullable: nullables) {
            for (auto &production: productions) {
                for (std::string right: production.second) {
                    if (right.find(nullable) != std::string::npos && right != nullable) {
                        std::size_t index = right.find(nullable);
                        std::string newRule = right.erase(index, nullable.length() + 1);
                        production.second.insert(newRule);
                    }
                }
            }
        }
    }

    int epsCount = countProductions();
    std::cout << "  Created " << epsCount << " productions, original had " << originalCount << "\n" << std::endl;
    print(std::cout);
    std::cout << std::endl;

    //2. Eliminate all unit pairs
    std::cout << " >> Eliminating unit pairs" << std::endl;

    std::map<std::string, std::set<std::string>> unitPairs;


    // Add all the basic pairs
    for (const std::string &var: variables) {
        unitPairs[var].insert(var);
    }

    std::map<std::string, std::set<std::string>> prodCopy = productions;
    int unitProdsCount = 0;
    for (auto &production: productions) {
        for (const std::string &right: production.second) {
            if (variables.find(production.first) != variables.end() && variables.find(right) != variables.end()) {
                unitPairs[production.first].insert(right);
                prodCopy[production.first].erase(right);
                unitProdsCount++;
            }
        }
    }
    productions = prodCopy;

    std::cout << "  Found " << unitProdsCount << " unit productions" << std::endl;

    bool addedSomething = true;
    while (addedSomething) {
        addedSomething = false;
        for (auto &pair1: unitPairs) {
            for (const std::string &right1: pair1.second) {
                for (const auto &pair2: unitPairs) {
                    for (const std::string &right2: pair2.second) {
                        if (right1 == pair2.first) {
                            if (pair1.second.insert(right2).second) addedSomething = true;
                        }
                    }
                }
            }
        }
    }


    std::cout << "  Unit pairs: {";
    for (auto it1 = unitPairs.begin(); it1 != unitPairs.end(); it1++) {
        for (auto it2 = it1->second.begin(); it2 != it1->second.end(); it2++) {
            if (it1 != unitPairs.begin() || it2 != it1->second.begin())std::cout << ", ";
            std::cout << "(" << it1->first << ", " << *it2 << ")";
        }
    }
    std::cout << "}" << std::endl;


    for (const auto &pair: unitPairs) {
        for (const std::string &second: pair.second) {
            for (const auto &prod: productions) {
                for (const std::string &right: prod.second) {
                    if (second == prod.first) {
                        productions[pair.first].insert(right);
                    }
                }
            }
        }
    }


    int unitCount = countProductions();
    std::cout << "  Created " << unitCount << " new productions, original had " << epsCount << "\n" << std::endl;
    print(std::cout);
    std::cout << std::endl;

    // Eliminate all useless symbols
    std::cout << " >> Eliminating useless symbols" << std::endl;
    // Determine generating symbols
    std::set<std::string> generatingSymbols;

    for (const std::string &term: terminals) generatingSymbols.insert(term);

    bool addedGeneratingSymbol = true;
    bool isGenerating;
    while (addedGeneratingSymbol) {
        addedGeneratingSymbol = false;
        for (const auto &prod: productions) {
            for (const std::string &right: prod.second) {
                isGenerating = true;
                for (char c: right) {
                    if (c == ' ') continue;
                    if (generatingSymbols.find(std::string(1, c)) == generatingSymbols.end()) isGenerating = false;
                }
                if (isGenerating) {
                    if (generatingSymbols.insert(prod.first).second) addedGeneratingSymbol = true;
                }
            }
        }
    }


    std::cout << "  Generating symbols: {";
    for (auto it = generatingSymbols.begin(); it != generatingSymbols.end(); ++it) {
        if (it != generatingSymbols.begin())std::cout << ", ";
        std::cout << *it;
    }
    std::cout << "}" << std::endl;

    // Delete all rules with non-generating symbols
    int prodsCountBefore = countProductions();

    std::map<std::string, std::set<std::string>> prodsCopy1 = productions;
    for (auto &production: productions) {
        for (const std::string &right: production.second) {
            for (char c: right) {
                if (c == ' ') continue;
                if (generatingSymbols.find(std::string(1, c)) == generatingSymbols.end()) {
                    prodsCopy1[production.first].erase(right);
                }
            }
        }
    }
    productions = prodsCopy1;

    // Determine reachable symbols
    std::set<std::string> reachableSymbols;

    reachableSymbols.insert(startSymbol);

    bool addedReachableSymbol = true;
    while (addedReachableSymbol) {
        addedReachableSymbol = false;
        for (const auto &prod: productions) {
            if (reachableSymbols.find(prod.first) != reachableSymbols.end()) {
                for (const std::string &right: prod.second) {
                    for (char c: right) {
                        if (c == ' ') continue;
                        if (reachableSymbols.insert(std::string(1, c)).second) addedReachableSymbol = true;
                    }
                }
            }
        }
    }

    std::cout << "  Reachable symbols: {";
    for (auto it = reachableSymbols.begin(); it != reachableSymbols.end(); ++it) {
        if (it != reachableSymbols.begin())std::cout << ", ";
        std::cout << *it;
    }
    std::cout << "}" << std::endl;

    // Delete all rules with unreachable symbols
    std::map<std::string, std::set<std::string>> prodsCopy2 = productions;
    for (auto &production: productions) {
        if (reachableSymbols.find(production.first) == reachableSymbols.end()) {
            prodsCopy2.erase(production.first);
            continue;
        }
        for (const std::string &right: production.second) {
            for (char c: right) {
                if (c == ' ') continue;
                if (reachableSymbols.find(std::string(1, c)) == reachableSymbols.end()) {
                    prodsCopy2[production.first].erase(right);
                }
            }
        }
    }
    productions = prodsCopy2;

    // Determine useful symbols
    std::set<std::string> usefulSymbols;
    for (const auto &prod: productions) {
        usefulSymbols.insert(prod.first);
        for (const std::string &right: prod.second) {
            for (char c: right) {
                if (c == ' ') continue;
                usefulSymbols.insert(std::string(1, c));
            }
        }
    }


    std::cout << "  Useful symbols: {";
    for (auto it = usefulSymbols.begin(); it != usefulSymbols.end(); ++it) {
        if (it != usefulSymbols.begin())std::cout << ", ";
        std::cout << *it;
    }
    std::cout << "}" << std::endl;

    std::set<std::string> usefulVars;
    std::set<std::string> usefulTerms;
    for (std::string symbol: usefulSymbols) {
        if (variables.find(symbol) != variables.end()) usefulVars.insert(symbol);
        else usefulTerms.insert(symbol);
    }


    std::cout << "  Removed " << variables.size() - usefulVars.size() << " variables, "
              << terminals.size() - usefulTerms.size() << " terminals and " << prodsCountBefore - countProductions()
              << " productions\n" << std::endl;

    variables = usefulVars;
    terminals = usefulTerms;

    print(std::cout);
    std::cout << std::endl;

    // Arrange bodies of length >= 2 to consist of only variables
    int prodsCountBeforeBodyVars = countProductions();
    std::cout << " >> Replacing terminals in bad bodies" << std::endl;

    int addedVarsCount = 0;
    int termCount = 0;
    int varCount = 0;
    for (std::pair<const std::string, std::set<std::string>> &prod: productions) {
        std::set<std::string> newRules;
        for (const std::string &right: prod.second) {
            termCount = 0;
            varCount = 0;
            for (char c: right) {
                if (c == ' ') continue;
                if (terminals.find(std::string(1, c)) != terminals.end()) termCount++;
                if (variables.find(std::string(1, c)) != variables.end()) varCount++;
            }
            if (varCount + termCount > 1) {
                std::string newRight = right;
                for (char c: right) {
                    if (terminals.find(std::string(1, c)) != terminals.end()) {
                        newRight = std::regex_replace(newRight, std::regex(std::string(1, c)),
                                                      getLeft(std::string(1, c), addedVarsCount));
                    }
                }
                newRules.insert(newRight);
            } else newRules.insert(right);
        }
        productions[prod.first] = newRules;
    }


    std::cout << "  Added " << addedVarsCount << " new variables: {";
    bool foundFirstVar = false;
    for (std::string var: variables) {
        if (foundFirstVar) std::cout << ", ";
        if (var.at(0) == '_') {
            std::cout << var;
            foundFirstVar = true;
        }
    }
    std::cout << "}" << std::endl;
    std::cout << "  Created " << countProductions() << " new productions, original had "
              << prodsCountBeforeBodyVars << "\n" << std::endl;

    print(std::cout);

    // Break bodies of length >= 3
    int bodiesBroken = 0;
    int varsAdded = 0;
    std::map<std::string, std::set<std::string> > newProds = productions;
    for (const auto &prod: productions) {
        for (const std::string &right: prod.second) {
            std::vector<std::string> symbols = getSymbols(right);
            if (symbols.size() >= 3) {
                bodiesBroken++;
                newProds[prod.first].erase(right);
                varsAdded++;
                std::pair<std::string, std::set<std::string>> newRule1;
                newRule1.first = getNextFreeVar(prod.first);
                newRule1.second.insert(symbols[symbols.size() - 2] + " " + symbols[symbols.size() - 1]);
                newProds.insert(newRule1);

                newProds[prod.first].insert(symbols[0] + " " + newRule1.first);
            }
        }
    }
    productions = newProds;


    std::cout << "\n >> Broke " << bodiesBroken << " bodies, added " << varsAdded << " new variables" << std::endl;
    std::cout << ">>> Result CFG:\n" << std::endl;

    print(std::cout);

}

int CFG::countProductions() const {
    int count = 0;
    for (const auto &p: productions) {
        count += p.second.size();
    }
    return count;
}

std::string CFG::getLeft(const std::string &r, int &addedVarsCount) {
    for (const auto &prod: productions) {
        for (const std::string &right: prod.second) {
            if (right == r) return prod.first;
        }
    }
    addedVarsCount++;
    std::pair<std::string, std::set<std::string>> newRule;
    newRule.first = "_" + r;
    variables.insert(newRule.first);
    newRule.second.insert(r);
    productions.insert(newRule);
    return newRule.first;
}

std::set<std::string> CFG::getLeft(const std::string &r) {
    std::set<std::string> result;
    for (const auto &prod: productions) {
        for (const auto &right: prod.second) {
            if (right == r) {
                result.insert(prod.first);
            }
        }
    }
    return result;
};

std::string CFG::getNextFreeVar(const std::string varName) {
    int i = 2;
    while (true) {
        if (variables.find(varName + "_" + std::to_string(i)) == variables.end()) {
            variables.insert(varName + "_" + std::to_string(i));
            return {varName + "_" + std::to_string(i)};
        }
        i++;
    }
}

std::vector<std::string> CFG::getSymbols(std::string str) {
    std::vector<std::string> symbols = {};
    for (std::string::size_type i = 0; i < str.length(); i++) {
        if (str.at(i) == ' ') continue;
        std::string curSymbol = "";
        while (i < str.length() && str.at(i) != ' ') {
            curSymbol += str.at(i);
            i++;
        }
        symbols.push_back(curSymbol);
    }
    return symbols;
}

bool CFG::accepts(const std::string input) {
    std::map<int, std::vector<std::set<std::string>>> table;
    for (std::string::size_type row = 0; row <= input.length() - 1; ++row) {
        table.insert({0, {}});
        if (row == 0) {
            for (char c: input) {
                std::set<std::string> symbols = getLeft(std::string(1, c));
                table[0].push_back(symbols);
            }
        } else {
            for (std::string::size_type col = 0; col < input.length() - row; ++col) {
                std::set<std::string> symbols;
                for (std::string::size_type i = 0; i < row; ++i) {
                    std::set<std::string> matches = match(table, col, row, i);
                    for (const std::string &match: matches) {
                        std::set<std::string> newSymbols = getLeft(match);
                        symbols.insert(newSymbols.begin(), newSymbols.end());
                    }
                }
                table[row].push_back(symbols);
            }
        }
    }
    printTable(table);
    if (table[input.length()-1][0].find(startSymbol) != table[input.length()-1][0].end()) {
        std::cout << "true" << std::endl;
    } else {
        std::cout << "false" << std::endl;
    }
}

std::set<std::string> CFG::match(std::map<int, std::vector<std::set<std::string>>> &table, int col, int row, int i) {
    std::set<std::string> result;
    std::set<std::string> left = table[i][col];
    std::set<std::string> right = table[row - 1 - i][col + i + 1];
    for (std::string l: left) {
        for (std::string r: right) {
            result.insert(l + " " + r);
        }
    }
    return result;
}

void CFG::printTable(std::map<int, std::vector<std::set<std::string>>> &table) {
    for (auto it = table.rbegin(); it != table.rend(); ++it) {
        std::cout << "| ";
        for (std::set<std::string> symbolsSet: it->second) {
            std::cout << "{";
            for (auto sIt=symbolsSet.begin(); sIt != symbolsSet.end(); sIt++) {
                if (sIt != symbolsSet.begin()) std::cout << ", ";
                std::cout << *sIt;
            }
            std::cout << "} | ";
        }
        std::cout << std::endl;
    }
}


