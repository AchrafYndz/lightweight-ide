#ifndef IDE_SRC_AUTOMATA_VALUE_H
#define IDE_SRC_AUTOMATA_VALUE_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

class Value {
private:
    std::string name;
    bool isTerminal = false;

public:
    Value(std::string name, bool isTerminal);
    const std::string& getName() const;
    bool getIsTerminal() { return isTerminal; };
};

using Values = std::map<std::string, Value*>;

#endif // IDE_SRC_AUTOMATA_VALUE_H
