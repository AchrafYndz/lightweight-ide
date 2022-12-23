#include "lexer.h"

Lexer::NextToken Lexer::get_next_token() {
    // Try to take as much characters as possible from the scanner so that the input keeps matching a regex.

    if (std::get<2>(this->scanner.peek_next_char()) == '\0') {
        // return `EOF` token without advancing the scanner
        const auto [row, col, next_char] = this->scanner.peek_next_char();

        return {TokenType::Eof, {{row, col}, {row, col}, "EOF"}};
    }

    NextToken result{};

    // TODO: make this better
    // set correct start location
    std::get<0>(result.second) = {std::get<0>(this->scanner.peek_next_char()),
                                  std::get<1>(this->scanner.peek_next_char())};

    bool matched = true;
    while (matched) {
        matched = false;
        const auto [row, col, next_char] = this->scanner.peek_next_char();

        if (next_char == '\0')
            break;

        // try to match the current built token to a matcher
        for (const auto& [token_type, matcher] : this->token_matchers) {
            if (!std::regex_match(std::get<2>(result.second) + next_char, matcher))
                continue;

            matched = true;
            // set the `token_type`
            result.first = token_type;

            // add the new char to result and update locations
            std::get<2>(result.second) += std::get<2>(this->scanner.get_next_char());
            std::get<1>(result.second) = {row, col};
            break;
        }
    }

    // TODO: add exception
    if (std::get<2>(result.second).empty())
        throw "invalid input";

    return result;
}
