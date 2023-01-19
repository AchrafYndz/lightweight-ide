#ifndef IDE_SRC_PARSER_SCANNER_H
#define IDE_SRC_PARSER_SCANNER_H

#include "stream_reader.h"

#include <tuple>

class Scanner {
public:
    using NextChar = std::tuple<unsigned int, unsigned int, char>;

public:
    inline Scanner(StreamReader reader) : reader(reader) {}

    /// Returns `{row, col, character}` of the next character.
    /// Note: Both `row` and `col` use zero-based indexing.
    NextChar get_next_char();

    /// Returns `{row, col, character}` of the next character without advancing the internal buffer and counters.
    /// Note: Both `row` and `col` use zero-based indexing.
    NextChar peek_next_char() const;

private:
    /// Note: Is mutable because of the implementation of `StreamReader::peek`.
    mutable StreamReader reader;

    unsigned int row{0}, col{0};
    unsigned int index{0};
};

#endif // IDE_SRC_PARSER_SCANNER_H
