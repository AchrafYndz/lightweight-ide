#include "doctest/doctest.h"

#include "parser/lexer.h"
#include "parser/scanner.h"

#include <fstream>
#include <iostream>
#include <vector>

/// help function to format `{row, col, character}` vector of tuples
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
