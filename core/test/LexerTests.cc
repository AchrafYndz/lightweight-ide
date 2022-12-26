#include "doctest/doctest.h"

#include "parser/lexer.h"
#include "parser/scanner.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

/// Helper function to format `{row, col, character}` vector of tuples.
std::ostream& operator<<(std::ostream& out, const std::vector<Scanner::NextChar>& v) {
    out << '{';

    for (auto it = v.begin(); it != v.end(); ++it) {
        out << '(';

        out << '`' << std::get<0>(*it) << "`,`" << std::get<1>(*it) << "`,`" << std::get<2>(*it) << '`';

        out << ((std::next(it) == v.end()) ? ")" : "),");
    }

    out << "}\n";

    return out;
}

/// helper function to format `{token_type, {{row_start, col_start}, {row_end, col_end}, token}}`
std::ostream& operator<<(std::ostream& out, const std::vector<Lexer::NextToken>& v) {
    out << '{';

    for (auto it = v.begin(); it != v.end(); ++it) {
        out << '{';

        // out << (std::vector<const char*>{"Comment", "Keyword", "Literal", "Punctuation", "Identifier", "Whitespace",
        //                                  "Eof",
        //                                  "Incorrect"}[static_cast<unsigned int>(it->first)]); // used for debugging
        out << static_cast<int>(it->first);

        out << ',';

        out << '(';

        out << '{' << '`' << std::get<0>(it->second).first << '`' << ',' << '`' << std::get<0>(it->second).second << '`'
            << "},";
        out << '{' << '`' << std::get<1>(it->second).first << '`' << ',' << '`' << std::get<1>(it->second).second << '`'
            << "},";
        out << '`' << std::get<2>(it->second) << '`';

        out << ')';

        out << ((std::next(it) == v.end()) ? "}" : "},");
    }

    out << "}\n";

    return out;
}

TEST_SUITE("ScannerTests") {
    TEST_CASE("[ScannerTests] peek_next_char") {
        Scanner scanner(StreamReader("test/res/input/scanner.txt"));

        auto start = scanner.peek_next_char();
        for (unsigned int i = 0; i < 100; ++i)
            CHECK_EQ(start, scanner.peek_next_char());
    }

    TEST_CASE("[ScannerTests] get_next_char") {
        Scanner scanner(StreamReader("test/res/input/scanner.txt"));

        std::ifstream expected_fstream("test/res/expected/ScannerTests-get_next_char.txt");
        std::stringstream expected{};
        expected << expected_fstream.rdbuf();

        std::stringstream actual{};

        // std::ofstream reset("test/res/expected/ScannerTests-get_next_char.txt");

        std::vector<Scanner::NextChar> v{};

        while (std::get<2>(scanner.peek_next_char()) != '\0') {
            v.push_back(scanner.get_next_char());
        }

        actual << v;

        CHECK_EQ(expected.str(), actual.str());
    }
}

TEST_SUITE("LexerTests") {
    TEST_CASE("[LexerTests] factorial") {
        Lexer lexer(Scanner(StreamReader("test/res/input/factorial.bro")));

        std::ifstream expected_fstream("test/res/expected/LexerTests-factorial.txt");
        std::stringstream expected{};
        expected << expected_fstream.rdbuf();

        std::stringstream actual{};

        // std::ofstream reset("test/res/expected/LexerTests-factorial.txt");

        std::vector<Lexer::NextToken> v{};
        Lexer::NextToken next_token{};

        do {
            next_token = lexer.get_next_token();
            v.push_back(next_token);
        } while (next_token.first != Lexer::TokenType::Eof);

        actual << v;

        CHECK_EQ(expected.str(), actual.str());
    }

    TEST_CASE("[LexerTests] all") {
        Lexer lexer(Scanner(StreamReader("test/res/input/all.bro")));

        std::ifstream expected_fstream("test/res/expected/LexerTests-all.txt");
        std::stringstream expected{};
        expected << expected_fstream.rdbuf();

        std::stringstream actual{};

        // std::ofstream reset("test/res/expected/LexerTests-all.txt");

        std::vector<Lexer::NextToken> v{};
        Lexer::NextToken next_token{};

        do {
            next_token = lexer.get_next_token();
            v.push_back(next_token);
        } while (next_token.first != Lexer::TokenType::Eof);

        actual << v;

        CHECK_EQ(expected.str(), actual.str());
    }
}
