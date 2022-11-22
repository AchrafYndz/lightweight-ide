#include <string>

#ifndef MB_ING_OEF1_PDAINPUT_H
#define MB_ING_OEF1_PDAINPUT_H


class PDAInput {
    std::string state;
    std::string input;
    std::string stackTop;
public:
    PDAInput(std::string state, std::string input, std::string stackTop);

    const std::string &getState() const;

    const std::string &getInput() const;

    const std::string &getStackTop() const;
};


#endif //MB_ING_OEF1_PDAINPUT_H
