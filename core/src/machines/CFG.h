#ifndef CFG_CFG_H
#define CFG_CFG_H

#include <set>
#include <vector>
#include <string>
#include <map>

class CFG {
private:
    std::set<std::string> variables;
    std::set<std::string> terminals;
    std::map<std::string, std::set<std::string>, std::less<std::string>> productions;
    std::string startSymbol;

    std::string getLeft(const std::string& r, int& addedVarsCount);
    std::set<std::string> getLeft(const std::string& r);

    std::set<std::string> match(std::map<int, std::vector<std::set<std::string>>>& table, int col, int row, int i);

    std::string getNextFreeVar(const std::string varName);

    int countProductions() const;

    std::vector<std::string> getSymbols(std::string str);

    void printTable(std::map<int, std::vector<std::set<std::string>>>& table);

public:
    // Constructors
    CFG();
    CFG(const std::string &filename);


    void toCNF();
    void print(std::ostream &out);

    bool accepts(const std::string input);
};


#endif //CFG_CFG_H
;