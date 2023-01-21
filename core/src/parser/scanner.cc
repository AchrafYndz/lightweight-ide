#include "scanner.h"

std::tuple<unsigned int, unsigned int, char> Scanner::get_next_char() {
    // TODO: make this better
    char next_char;
    try {
        next_char = this->reader.consume(this->index);
    } catch (StreamReader::EOFException&) {
        next_char = this->end_of_file;
    }
    const auto result = std::make_tuple(this->row, this->col, next_char);

    // increment counters
    if (next_char == '\n') {
        ++this->row;
        this->col = 0;
    } else {
        ++this->col;
    }

    ++this->index;

    return result;
}

Scanner::NextChar Scanner::peek_next_char() const {
    // TODO: make this better
    char next_char;
    try {
        next_char = this->reader.peek(this->index);
    } catch (StreamReader::EOFException&) {
        next_char = this->end_of_file;
    }
    return std::make_tuple(this->row, this->col, next_char);
}
