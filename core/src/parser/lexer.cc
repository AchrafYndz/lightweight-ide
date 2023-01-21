#include "lexer.h"

const std::unordered_map<std::string, Lexer::TokenType> Lexer::terminal_to_token_type{
    {"{", Lexer::TokenType::LeftCurly},
    {"}", Lexer::TokenType::RightCurly},
    {"(", Lexer::TokenType::LeftBracket},
    {")", Lexer::TokenType::RightBracket},
    {"+", Lexer::TokenType::Plus},
    {"-", Lexer::TokenType::Min},
    {"*", Lexer::TokenType::Mult},
    {"/", Lexer::TokenType::Div},
    {"<", Lexer::TokenType::Less},
    {">", Lexer::TokenType::Greater},
    {"=", Lexer::TokenType::Equals},
    {"!", Lexer::TokenType::Not},
    {";", Lexer::TokenType::SemiColon},
    {std::string(1, Lexer::eof_token), Lexer::TokenType::Eof},
    {"identifier", Lexer::TokenType::Identifier},
    {"keyword", Lexer::TokenType::Keyword},
    {"literal", Lexer::TokenType::Literal}};

Lexer::NextToken Lexer::get_next_token() {
    // Try to take as much characters as possible from the scanner so that the input keeps matching a regex.

    if (std::get<2>(this->scanner.peek_next_char()) == '\0') {
        // return `EOF` token without advancing the scanner
        const auto [row, col, next_char] = this->scanner.peek_next_char();

        return {TokenType::Eof, {row, col}, {row, col}, "EOF"};
    }

    // TODO: make this better
    // set correct start location
    const Pos token_start = {std::get<0>(this->scanner.peek_next_char()), std::get<1>(this->scanner.peek_next_char())};

    std::string token_value{};
    TokenType token_type{};
    Pos token_end{};

    bool matched = true;
    while (matched) {
        matched = false;
        const auto [row, col, next_char] = this->scanner.peek_next_char();

        if (next_char == this->eof_token)
            break;

        // try to match the current built token to a matcher
        for (const auto& matcher_pair : this->token_matchers) {
            const auto& matcher = matcher_pair.second;
            if (!std::regex_match(token_value + next_char, matcher))
                continue;

            TokenType matcher_token_type = matcher_pair.first;
            // match punctuation
            if (matcher_token_type == TokenType::Punctuation) {
                assert(token_value.length() == 0 && "length of punctuation token is 1");

                matcher_token_type = this->terminal_to_token_type.at(std::string(1, next_char));
            }

            matched = true;
            // set the `token_type`
            token_type = matcher_token_type;

            // add the new char to result and update locations
            token_value += std::get<2>(this->scanner.get_next_char());
            token_end = {row, col};
            break;
        }
    }

    if (token_value.empty())
        return {TokenType::Incorrect, token_start, token_end,
                std::string(1, std::get<2>(this->scanner.get_next_char()))};

    return {token_type, token_start, token_end, token_value};
}
