#include "lexer.h"

const std::unordered_map<char, Lexer::TokenType> Lexer::character_to_token_type{
    {'{', Lexer::TokenType::LeftCurly},   {'}', Lexer::TokenType::RightCurly},
    {'(', Lexer::TokenType::LeftBracket}, {')', Lexer::TokenType::RightBracket},
    {'+', Lexer::TokenType::Plus},        {'-', Lexer::TokenType::Min},
    {'*', Lexer::TokenType::Mult},        {'/', Lexer::TokenType::Div},
    {'<', Lexer::TokenType::Less},        {'>', Lexer::TokenType::Greater},
    {'=', Lexer::TokenType::Equals},      {'!', Lexer::TokenType::Not},
    {';', Lexer::TokenType::SemiColon},   {Lexer::eof_token, Lexer::TokenType::Eof},
};

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

        if (next_char == this->eof_token)
            break;

        // try to match the current built token to a matcher
        for (const auto& matcher_pair : this->token_matchers) {
            const auto& matcher = matcher_pair.second;
            if (!std::regex_match(std::get<2>(result.second) + next_char, matcher))
                continue;

            TokenType token_type = matcher_pair.first;
            // match punctuation
            if (token_type == TokenType::Punctuation) {
                assert(std::get<2>(result.second).length() == 0 && "length of punctuation token is 1");

                token_type = this->character_to_token_type.at(next_char);
            }

            matched = true;
            // set the `token_type`
            result.first = token_type;

            // add the new char to result and update locations
            std::get<2>(result.second) += std::get<2>(this->scanner.get_next_char());
            std::get<1>(result.second) = {row, col};
            break;
        }
    }

    if (std::get<2>(result.second).empty())
        return {TokenType::Incorrect,
                {std::get<0>(result.second), std::get<1>(result.second),
                 std::string(1, std::get<2>(this->scanner.get_next_char()))}};

    return result;
}
