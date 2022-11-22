#include <iostream>
#include <string>
#include <vector>
#include <map>

#ifndef MB_ING_OEF1_VALUE_H
#define MB_ING_OEF1_VALUE_H


class Value {
private:
    std::string name;
    bool isTerminal = false;
public:
    Value(std::string name, bool isTerminal);
    const std::string &getName() const;
    bool getIsTerminal() { return isTerminal; };
};

typedef std::map<std::string, Value*> Values;


#endif //MB_ING_OEF1_VALUE_H
