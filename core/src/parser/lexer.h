#ifndef IDE_SRC_PARSER_LEXER_H
#define IDE_SRC_PARSER_LEXER_H

#include "scanner.h"

#include <map>
#include <regex>
#include <string>
#include <tuple>

class Lexer {
public:
    /// Token types the lexer uses to group characters.
    enum class TokenType {
        /// The order in which they are defined, defines the precedence.

        Comment,
        Keyword,
        Literal,
        Punctuation,
        Identifier,
        Whitespace,
        Eof,
    };

public:
    /// Token descriptor of the form `{token_type, ({row_start, col_start}, {row_end, col_end}, token)}`.
    using NextToken = std::pair<TokenType, std::tuple<std::pair<unsigned int, unsigned int>,
                                                      std::pair<unsigned int, unsigned int>, std::string>>;

public:
    inline Lexer(Scanner scanner) : scanner(scanner) {}

    /// Returns `{token_type, ({row_start, col_start}, {row_end, col_end}, token)}` .
    /// With `token` being the largest possible grouping of characters that matches a token descriptor.
    NextToken get_next_token();

private:
    Scanner scanner;

    /// Hardcoded list of regex matchers for different tokens in the language
    /// Note: The precedence of the token is defined by the order in the enum type.
    const std::map<TokenType, std::regex> token_matchers{
        {TokenType::Comment, std::regex("//.*|/\\*.*\\*/")},
        {TokenType::Keyword, std::regex("if|else|return|while|fn")},
        {TokenType::Literal, std::regex("\\d+")},
        {TokenType::Identifier, std::regex("[[:alpha:]]\\w*")},
        {TokenType::Punctuation, std::regex("\\{|\\}|\\(|\\)|\\+|-|\\*|/|<|>|=|;")},
        {TokenType::Whitespace, std::regex("\\s+")},
    };
};

#endif // IDE_SRC_PARSER_LEXER_H
