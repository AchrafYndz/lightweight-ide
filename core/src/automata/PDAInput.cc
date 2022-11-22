#include "PDAInput.h"

PDAInput::PDAInput(std::string state, std::string input, std::string stackTop) {
    PDAInput::state = state;
    PDAInput::input = input;
    PDAInput::stackTop = stackTop;
}

const std::string &PDAInput::getState() const {
    return state;
}

const std::string &PDAInput::getInput() const {
    return input;
}

const std::string &PDAInput::getStackTop() const {
    return stackTop;
}
