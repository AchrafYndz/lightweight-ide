#include "Value.h"

Value::Value(std::string name, bool isTerminal) {
    Value::name = name;
    Value::isTerminal = isTerminal;
}

const std::string& Value::getName() const { return name; }
