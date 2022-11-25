#ifndef IDE_SRC_AUTOMATA_PDAINPUT_H
#define IDE_SRC_AUTOMATA_PDAINPUT_H

#include <string>

class PDAInput {
    std::string state;
    std::string input;
    std::string stackTop;

public:
    PDAInput(std::string state, std::string input, std::string stackTop);

    const std::string& getState() const;

    const std::string& getInput() const;

    const std::string& getStackTop() const;
};

#endif // IDE_SRC_AUTOMATA_PDAINPUT_H
