#ifndef IDE_SRC_PARSER_LEXER_H
#define IDE_SRC_PARSER_LEXER_H

#include "machines/CFG.h"
#include "scanner.h"

#include <map>
#include <regex>
#include <string>
#include <tuple>
#include <unordered_map>

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
        Incorrect,

        /// Punctuation characters
        LeftCurly,
        RightCurly,
        LeftBracket,
        RightBracket,
        Plus,
        Min,
        Mult,
        Div,
        Less,
        Greater,
        Equals,
        Not,
        SemiColon,
    };

    static constexpr char eof_token{'\0'};

public:
    using Pos = std::pair<unsigned long, unsigned long>;

public:
    /// Token descriptor emitted by the lexer.
    struct NextToken {
        /// Type of the token.
        TokenType type{};
        /// Start position.
        Pos start{};
        /// End position.
        Pos end{};
        /// Value of the token.
        std::string value{};
    };

public:
    inline Lexer(Scanner scanner) : scanner(scanner) {}

    /// Returns and consumes the characters needed for next token. With `token` being the largest possible grouping of
    /// characters that matches a token descriptor.
    NextToken get_next_token();

public:
    /// Character lookup table used to convert characters into `Lexer::TokenType`.
    static const std::unordered_map<char, TokenType> character_to_token_type;

private:
    Scanner scanner;

    /// Hardcoded list of regex matchers for different tokens in the language
    /// Note: The precedence of the token is defined by the order in the enum type.
    const std::map<TokenType, std::regex> token_matchers{
        {Lexer::TokenType::Comment, std::regex("//.*")},
        {Lexer::TokenType::Keyword, std::regex("if|else|return|while|fn|let")},
        {Lexer::TokenType::Literal, std::regex("\\d+")},
        {Lexer::TokenType::Identifier, std::regex("[[:alpha:]]\\w*")},
        {Lexer::TokenType::Punctuation, std::regex("\\{|\\}|\\(|\\)|\\+|-|\\*|/|<|>|=|!|;")},
        {Lexer::TokenType::Whitespace, std::regex("\\s+")},
    };
};

#endif // IDE_SRC_PARSER_LEXER_H
