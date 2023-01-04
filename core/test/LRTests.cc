#include "doctest/doctest.h"

#include "machines/CFG.h"
#include "parser/lr.h"

class LRTest {
protected:
    LR lr{};

protected:
    LR::ItemSet closure(const LR::ItemSet& item, const std::string& separator, const CFG& cfg) {
        return LR::closure(item, separator, cfg);
    }

    void setup(const CFG& cfg) { this->lr = LR(cfg); }

    const LR::ParsingTable& get_table() const { return this->lr.table; }
};

TEST_SUITE("LRTests") {
    TEST_CASE_FIXTURE(LRTest, "[LRTests] closure") {
        const std::string separator("·");
        const LR::ItemSet start_item_set{{LR::Item{"S", {separator, "A", "A"}}}};

        const CFG cfg("./test/res/input/CFG2.json");

        CHECK_EQ(closure(start_item_set, separator, cfg),
                 LR::ItemSet{{{"A", {"·", "a", "A"}}, {"A", {"·", "b"}}, {"S", {"·", "A", "A"}}}});
    }

    TEST_CASE_FIXTURE(LRTest, "[LRTests] parsing_table") {
        const CFG cfg("./test/res/input/CFG2.json");
        setup(cfg);

        LR::ParsingTable table{
            {0, {{{"a", {LR::ActionType::Shift, 1}}, {"b", {LR::ActionType::Shift, 2}}}, {{"A", 3}, {"S", 4}}}},
            {1, {{{"a", {LR::ActionType::Shift, 1}}, {"b", {LR::ActionType::Shift, 2}}}, {{"A", 5}}}},
            {2,
             {{{"$", {LR::ActionType::Reduce, 1}},
               {"a", {LR::ActionType::Reduce, 1}},
               {"b", {LR::ActionType::Reduce, 1}}},
              {}}},
            {3, {{{"a", {LR::ActionType::Shift, 1}}, {"b", {LR::ActionType::Shift, 2}}}, {{"A", 6}}}},
            {4, {{{"$", {LR::ActionType::Accept, 0}}}, {}}},
            {5,
             {{{"$", {LR::ActionType::Reduce, 0}},
               {"a", {LR::ActionType::Reduce, 0}},
               {"b", {LR::ActionType::Reduce, 0}}},
              {}}},
            {6, {{{"$", {LR::ActionType::Reduce, 2}}}, {}}}};

        CHECK_EQ(table, get_table());
    }

    TEST_CASE_FIXTURE(LRTest, "[LRTests] table_edge_case_0") {
        const CFG cfg = CFG::parse_ebnf("test/res/input/EBNF0.txt");
        setup(cfg);

        const LR::ParsingTable table{
            {0, {{{"literal", {LR::ActionType::Shift, 3}}}, {{"<assignment>", 4}, {"<expression>", 1}, {"<term>", 2}}}},
            {1, {{{";", {LR::ActionType::Shift, 5}}}, {}}},
            {2, {{{"+", {LR::ActionType::Shift, 6}}, {";", {LR::ActionType::Reduce, 1}}}, {}}},
            {3,
             {{{"*", {LR::ActionType::Shift, 7}},
               {"+", {LR::ActionType::Reduce, 3}},
               {";", {LR::ActionType::Reduce, 3}}},
              {}}},
            {4, {{{"$", {LR::ActionType::Accept, 0}}}, {}}},
            {5, {{{"$", {LR::ActionType::Reduce, 0}}}, {}}},
            {6, {{{"literal", {LR::ActionType::Shift, 3}}}, {{"<expression>", 8}, {"<term>", 2}}}},
            {7, {{{"literal", {LR::ActionType::Shift, 3}}}, {{"<term>", 9}}}},
            {8, {{{";", {LR::ActionType::Reduce, 2}}}, {}}},
            {9, {{{"+", {LR::ActionType::Reduce, 4}}, {";", {LR::ActionType::Reduce, 4}}}, {}}}};

        CHECK_EQ(table, get_table());
    }

    TEST_CASE_FIXTURE(LRTest, "[LRTests] bro_parse_table") {
        const CFG cfg = CFG::parse_ebnf("test/res/input/EBNF2CFG.txt");
        setup(cfg);

        const LR::ParsingTable table{
            {0, {{{"fn", {LR::ActionType::Shift, 1}}}, {{"<function>", 2}, {"<program>", 3}}}},
            {1, {{{"identifier", {LR::ActionType::Shift, 4}}}, {}}},
            {2,
             {{{"$", {LR::ActionType::Reduce, 29}}, {"fn", {LR::ActionType::Shift, 1}}},
              {{"<function>", 2}, {"<program>", 5}}}},
            {3, {{{"$", {LR::ActionType::Accept, 0}}}, {}}},
            {4, {{{"(", {LR::ActionType::Shift, 6}}}, {}}},
            {5, {{{"$", {LR::ActionType::Reduce, 30}}}, {}}},
            {6,
             {{{")", {LR::ActionType::Shift, 8}}, {"identifier", {LR::ActionType::Shift, 7}}}, {{"<arguments>", 9}}}},
            {7, {{{")", {LR::ActionType::Reduce, 0}}, {",", {LR::ActionType::Shift, 10}}}, {}}},
            {8, {{{"{", {LR::ActionType::Shift, 11}}}, {}}},
            {9, {{{")", {LR::ActionType::Shift, 12}}}, {}}},
            {10, {{{"identifier", {LR::ActionType::Shift, 7}}}, {{"<arguments>", 13}}}},
            {11,
             {{{"identifier", {LR::ActionType::Shift, 14}},
               {"if", {LR::ActionType::Shift, 18}},
               {"let", {LR::ActionType::Shift, 15}},
               {"return", {LR::ActionType::Shift, 19}}},
              {{"<assignment>", 20},
               {"<body>", 17},
               {"<function_call>", 21},
               {"<if>", 22},
               {"<return>", 23},
               {"<statement>", 16}}}},
            {12, {{{"{", {LR::ActionType::Shift, 24}}}, {}}},
            {13, {{{")", {LR::ActionType::Reduce, 1}}}, {}}},
            {14, {{{"(", {LR::ActionType::Shift, 26}}, {"=", {LR::ActionType::Shift, 25}}}, {}}},
            {15, {{{"identifier", {LR::ActionType::Shift, 27}}}, {}}},
            {16,
             {{{"identifier", {LR::ActionType::Shift, 14}},
               {"if", {LR::ActionType::Shift, 18}},
               {"let", {LR::ActionType::Shift, 15}},
               {"return", {LR::ActionType::Shift, 19}},
               {"}", {LR::ActionType::Reduce, 4}}},
              {{"<assignment>", 20},
               {"<body>", 28},
               {"<function_call>", 21},
               {"<if>", 22},
               {"<return>", 23},
               {"<statement>", 16}}}},
            {17, {{{"}", {LR::ActionType::Shift, 29}}}, {}}},
            {18,
             {{{"!", {LR::ActionType::Shift, 32}},
               {"(", {LR::ActionType::Shift, 33}},
               {"-", {LR::ActionType::Shift, 34}},
               {"identifier", {LR::ActionType::Shift, 36}},
               {"literal", {LR::ActionType::Shift, 37}}},
              {{"<expression>", 30}, {"<factor>", 38}, {"<function_call>", 35}, {"<term>", 31}}}},
            {19,
             {{{"!", {LR::ActionType::Shift, 32}},
               {"(", {LR::ActionType::Shift, 33}},
               {"-", {LR::ActionType::Shift, 34}},
               {"identifier", {LR::ActionType::Shift, 36}},
               {"literal", {LR::ActionType::Shift, 37}}},
              {{"<expression>", 39}, {"<factor>", 38}, {"<function_call>", 35}, {"<term>", 31}}}},
            {20,
             {{{"identifier", {LR::ActionType::Reduce, 32}},
               {"if", {LR::ActionType::Reduce, 32}},
               {"let", {LR::ActionType::Reduce, 32}},
               {"return", {LR::ActionType::Reduce, 32}},
               {"}", {LR::ActionType::Reduce, 32}}},
              {}}},
            {21, {{{";", {LR::ActionType::Shift, 40}}}, {}}},
            {22,
             {{{"identifier", {LR::ActionType::Reduce, 34}},
               {"if", {LR::ActionType::Reduce, 34}},
               {"let", {LR::ActionType::Reduce, 34}},
               {"return", {LR::ActionType::Reduce, 34}},
               {"}", {LR::ActionType::Reduce, 34}}},
              {}}},
            {23,
             {{{"identifier", {LR::ActionType::Reduce, 35}},
               {"if", {LR::ActionType::Reduce, 35}},
               {"let", {LR::ActionType::Reduce, 35}},
               {"return", {LR::ActionType::Reduce, 35}},
               {"}", {LR::ActionType::Reduce, 35}}},
              {}}},
            {24,
             {{{"identifier", {LR::ActionType::Shift, 14}},
               {"if", {LR::ActionType::Shift, 18}},
               {"let", {LR::ActionType::Shift, 15}},
               {"return", {LR::ActionType::Shift, 19}}},
              {{"<assignment>", 20},
               {"<body>", 41},
               {"<function_call>", 21},
               {"<if>", 22},
               {"<return>", 23},
               {"<statement>", 16}}}},
            {25,
             {{{"!", {LR::ActionType::Shift, 32}},
               {"(", {LR::ActionType::Shift, 33}},
               {"-", {LR::ActionType::Shift, 34}},
               {"identifier", {LR::ActionType::Shift, 36}},
               {"literal", {LR::ActionType::Shift, 37}}},
              {{"<expression>", 42}, {"<factor>", 38}, {"<function_call>", 35}, {"<term>", 31}}}},
            {26,
             {{{"!", {LR::ActionType::Shift, 32}},
               {"(", {LR::ActionType::Shift, 33}},
               {")", {LR::ActionType::Shift, 44}},
               {"-", {LR::ActionType::Shift, 34}},
               {"identifier", {LR::ActionType::Shift, 36}},
               {"literal", {LR::ActionType::Shift, 37}}},
              {{"<call_arguments>", 45},
               {"<expression>", 43},
               {"<factor>", 38},
               {"<function_call>", 35},
               {"<term>", 31}}}},
            {27, {{{"=", {LR::ActionType::Shift, 46}}}, {}}},
            {28, {{{"}", {LR::ActionType::Reduce, 5}}}, {}}},
            {29, {{{"$", {LR::ActionType::Reduce, 23}}, {"fn", {LR::ActionType::Reduce, 23}}}, {}}},
            {30,
             {{{"!", {LR::ActionType::Shift, 47}},
               {"+", {LR::ActionType::Shift, 48}},
               {"-", {LR::ActionType::Shift, 49}},
               {"<", {LR::ActionType::Shift, 50}},
               {"=", {LR::ActionType::Shift, 51}},
               {">", {LR::ActionType::Shift, 52}},
               {"{", {LR::ActionType::Shift, 53}}},
              {}}},
            {31,
             {{{"!", {LR::ActionType::Reduce, 16}},
               {")", {LR::ActionType::Reduce, 16}},
               {"*", {LR::ActionType::Shift, 54}},
               {"+", {LR::ActionType::Reduce, 16}},
               {",", {LR::ActionType::Reduce, 16}},
               {"-", {LR::ActionType::Reduce, 16}},
               {"/", {LR::ActionType::Shift, 55}},
               {";", {LR::ActionType::Reduce, 16}},
               {"<", {LR::ActionType::Reduce, 16}},
               {"=", {LR::ActionType::Reduce, 16}},
               {">", {LR::ActionType::Reduce, 16}},
               {"{", {LR::ActionType::Reduce, 16}}},
              {}}},
            {32, {{{"(", {LR::ActionType::Shift, 56}}}, {}}},
            {33,
             {{{"!", {LR::ActionType::Shift, 32}},
               {"(", {LR::ActionType::Shift, 33}},
               {"-", {LR::ActionType::Shift, 34}},
               {"identifier", {LR::ActionType::Shift, 36}},
               {"literal", {LR::ActionType::Shift, 37}}},
              {{"<expression>", 57}, {"<factor>", 38}, {"<function_call>", 35}, {"<term>", 31}}}},
            {34,
             {{{"!", {LR::ActionType::Shift, 32}},
               {"(", {LR::ActionType::Shift, 33}},
               {"-", {LR::ActionType::Shift, 34}},
               {"identifier", {LR::ActionType::Shift, 36}},
               {"literal", {LR::ActionType::Shift, 37}}},
              {{"<factor>", 58}, {"<function_call>", 35}}}},
            {35,
             {{{"!", {LR::ActionType::Reduce, 20}},
               {")", {LR::ActionType::Reduce, 20}},
               {"*", {LR::ActionType::Reduce, 20}},
               {"+", {LR::ActionType::Reduce, 20}},
               {",", {LR::ActionType::Reduce, 20}},
               {"-", {LR::ActionType::Reduce, 20}},
               {"/", {LR::ActionType::Reduce, 20}},
               {";", {LR::ActionType::Reduce, 20}},
               {"<", {LR::ActionType::Reduce, 20}},
               {"=", {LR::ActionType::Reduce, 20}},
               {">", {LR::ActionType::Reduce, 20}},
               {"{", {LR::ActionType::Reduce, 20}}},
              {}}},
            {36,
             {{{"!", {LR::ActionType::Reduce, 21}},
               {"(", {LR::ActionType::Shift, 26}},
               {")", {LR::ActionType::Reduce, 21}},
               {"*", {LR::ActionType::Reduce, 21}},
               {"+", {LR::ActionType::Reduce, 21}},
               {",", {LR::ActionType::Reduce, 21}},
               {"-", {LR::ActionType::Reduce, 21}},
               {"/", {LR::ActionType::Reduce, 21}},
               {";", {LR::ActionType::Reduce, 21}},
               {"<", {LR::ActionType::Reduce, 21}},
               {"=", {LR::ActionType::Reduce, 21}},
               {">", {LR::ActionType::Reduce, 21}},
               {"{", {LR::ActionType::Reduce, 21}}},
              {}}},
            {37,
             {{{"!", {LR::ActionType::Reduce, 22}},
               {")", {LR::ActionType::Reduce, 22}},
               {"*", {LR::ActionType::Reduce, 22}},
               {"+", {LR::ActionType::Reduce, 22}},
               {",", {LR::ActionType::Reduce, 22}},
               {"-", {LR::ActionType::Reduce, 22}},
               {"/", {LR::ActionType::Reduce, 22}},
               {";", {LR::ActionType::Reduce, 22}},
               {"<", {LR::ActionType::Reduce, 22}},
               {"=", {LR::ActionType::Reduce, 22}},
               {">", {LR::ActionType::Reduce, 22}},
               {"{", {LR::ActionType::Reduce, 22}}},
              {}}},
            {38,
             {{{"!", {LR::ActionType::Reduce, 36}},
               {")", {LR::ActionType::Reduce, 36}},
               {"*", {LR::ActionType::Reduce, 36}},
               {"+", {LR::ActionType::Reduce, 36}},
               {",", {LR::ActionType::Reduce, 36}},
               {"-", {LR::ActionType::Reduce, 36}},
               {"/", {LR::ActionType::Reduce, 36}},
               {";", {LR::ActionType::Reduce, 36}},
               {"<", {LR::ActionType::Reduce, 36}},
               {"=", {LR::ActionType::Reduce, 36}},
               {">", {LR::ActionType::Reduce, 36}},
               {"{", {LR::ActionType::Reduce, 36}}},
              {}}},
            {39,
             {{{"!", {LR::ActionType::Shift, 47}},
               {"+", {LR::ActionType::Shift, 48}},
               {"-", {LR::ActionType::Shift, 49}},
               {";", {LR::ActionType::Shift, 59}},
               {"<", {LR::ActionType::Shift, 50}},
               {"=", {LR::ActionType::Shift, 51}},
               {">", {LR::ActionType::Shift, 52}}},
              {}}},
            {40,
             {{{"identifier", {LR::ActionType::Reduce, 33}},
               {"if", {LR::ActionType::Reduce, 33}},
               {"let", {LR::ActionType::Reduce, 33}},
               {"return", {LR::ActionType::Reduce, 33}},
               {"}", {LR::ActionType::Reduce, 33}}},
              {}}},
            {41, {{{"}", {LR::ActionType::Shift, 60}}}, {}}},
            {42,
             {{{"!", {LR::ActionType::Shift, 47}},
               {"+", {LR::ActionType::Shift, 48}},
               {"-", {LR::ActionType::Shift, 49}},
               {";", {LR::ActionType::Shift, 61}},
               {"<", {LR::ActionType::Shift, 50}},
               {"=", {LR::ActionType::Shift, 51}},
               {">", {LR::ActionType::Shift, 52}}},
              {}}},
            {43,
             {{{"!", {LR::ActionType::Shift, 47}},
               {")", {LR::ActionType::Reduce, 6}},
               {"+", {LR::ActionType::Shift, 48}},
               {",", {LR::ActionType::Shift, 62}},
               {"-", {LR::ActionType::Shift, 49}},
               {"<", {LR::ActionType::Shift, 50}},
               {"=", {LR::ActionType::Shift, 51}},
               {">", {LR::ActionType::Shift, 52}}},
              {}}},
            {44,
             {{{"!", {LR::ActionType::Reduce, 25}},
               {")", {LR::ActionType::Reduce, 25}},
               {"*", {LR::ActionType::Reduce, 25}},
               {"+", {LR::ActionType::Reduce, 25}},
               {",", {LR::ActionType::Reduce, 25}},
               {"-", {LR::ActionType::Reduce, 25}},
               {"/", {LR::ActionType::Reduce, 25}},
               {";", {LR::ActionType::Reduce, 25}},
               {"<", {LR::ActionType::Reduce, 25}},
               {"=", {LR::ActionType::Reduce, 25}},
               {">", {LR::ActionType::Reduce, 25}},
               {"{", {LR::ActionType::Reduce, 25}}},
              {}}},
            {45, {{{")", {LR::ActionType::Shift, 63}}}, {}}},
            {46,
             {{{"!", {LR::ActionType::Shift, 32}},
               {"(", {LR::ActionType::Shift, 33}},
               {"-", {LR::ActionType::Shift, 34}},
               {"identifier", {LR::ActionType::Shift, 36}},
               {"literal", {LR::ActionType::Shift, 37}}},
              {{"<expression>", 64}, {"<factor>", 38}, {"<function_call>", 35}, {"<term>", 31}}}},
            {47, {{{"=", {LR::ActionType::Shift, 65}}}, {}}},
            {48,
             {{{"!", {LR::ActionType::Shift, 32}},
               {"(", {LR::ActionType::Shift, 33}},
               {"-", {LR::ActionType::Shift, 34}},
               {"identifier", {LR::ActionType::Shift, 36}},
               {"literal", {LR::ActionType::Shift, 37}}},
              {{"<factor>", 38}, {"<function_call>", 35}, {"<term>", 66}}}},
            {49,
             {{{"!", {LR::ActionType::Shift, 32}},
               {"(", {LR::ActionType::Shift, 33}},
               {"-", {LR::ActionType::Shift, 34}},
               {"identifier", {LR::ActionType::Shift, 36}},
               {"literal", {LR::ActionType::Shift, 37}}},
              {{"<factor>", 38}, {"<function_call>", 35}, {"<term>", 67}}}},
            {50,
             {{{"!", {LR::ActionType::Shift, 32}},
               {"(", {LR::ActionType::Shift, 33}},
               {"-", {LR::ActionType::Shift, 34}},
               {"=", {LR::ActionType::Shift, 69}},
               {"identifier", {LR::ActionType::Shift, 36}},
               {"literal", {LR::ActionType::Shift, 37}}},
              {{"<factor>", 38}, {"<function_call>", 35}, {"<term>", 68}}}},
            {51, {{{"=", {LR::ActionType::Shift, 70}}}, {}}},
            {52,
             {{{"!", {LR::ActionType::Shift, 32}},
               {"(", {LR::ActionType::Shift, 33}},
               {"-", {LR::ActionType::Shift, 34}},
               {"=", {LR::ActionType::Shift, 72}},
               {"identifier", {LR::ActionType::Shift, 36}},
               {"literal", {LR::ActionType::Shift, 37}}},
              {{"<factor>", 38}, {"<function_call>", 35}, {"<term>", 71}}}},
            {53,
             {{{"identifier", {LR::ActionType::Shift, 14}},
               {"if", {LR::ActionType::Shift, 18}},
               {"let", {LR::ActionType::Shift, 15}},
               {"return", {LR::ActionType::Shift, 19}}},
              {{"<assignment>", 20},
               {"<body>", 73},
               {"<function_call>", 21},
               {"<if>", 22},
               {"<return>", 23},
               {"<statement>", 16}}}},
            {54,
             {{{"!", {LR::ActionType::Shift, 32}},
               {"(", {LR::ActionType::Shift, 33}},
               {"-", {LR::ActionType::Shift, 34}},
               {"identifier", {LR::ActionType::Shift, 36}},
               {"literal", {LR::ActionType::Shift, 37}}},
              {{"<factor>", 74}, {"<function_call>", 35}}}},
            {55,
             {{{"!", {LR::ActionType::Shift, 32}},
               {"(", {LR::ActionType::Shift, 33}},
               {"-", {LR::ActionType::Shift, 34}},
               {"identifier", {LR::ActionType::Shift, 36}},
               {"literal", {LR::ActionType::Shift, 37}}},
              {{"<factor>", 75}, {"<function_call>", 35}}}},
            {56,
             {{{"!", {LR::ActionType::Shift, 32}},
               {"(", {LR::ActionType::Shift, 33}},
               {"-", {LR::ActionType::Shift, 34}},
               {"identifier", {LR::ActionType::Shift, 36}},
               {"literal", {LR::ActionType::Shift, 37}}},
              {{"<expression>", 76}, {"<factor>", 38}, {"<function_call>", 35}, {"<term>", 31}}}},
            {57,
             {{{"!", {LR::ActionType::Shift, 47}},
               {")", {LR::ActionType::Shift, 77}},
               {"+", {LR::ActionType::Shift, 48}},
               {"-", {LR::ActionType::Shift, 49}},
               {"<", {LR::ActionType::Shift, 50}},
               {"=", {LR::ActionType::Shift, 51}},
               {">", {LR::ActionType::Shift, 52}}},
              {}}},
            {58,
             {{{"!", {LR::ActionType::Reduce, 19}},
               {")", {LR::ActionType::Reduce, 19}},
               {"*", {LR::ActionType::Reduce, 19}},
               {"+", {LR::ActionType::Reduce, 19}},
               {",", {LR::ActionType::Reduce, 19}},
               {"-", {LR::ActionType::Reduce, 19}},
               {"/", {LR::ActionType::Reduce, 19}},
               {";", {LR::ActionType::Reduce, 19}},
               {"<", {LR::ActionType::Reduce, 19}},
               {"=", {LR::ActionType::Reduce, 19}},
               {">", {LR::ActionType::Reduce, 19}},
               {"{", {LR::ActionType::Reduce, 19}}},
              {}}},
            {59,
             {{{"identifier", {LR::ActionType::Reduce, 31}},
               {"if", {LR::ActionType::Reduce, 31}},
               {"let", {LR::ActionType::Reduce, 31}},
               {"return", {LR::ActionType::Reduce, 31}},
               {"}", {LR::ActionType::Reduce, 31}}},
              {}}},
            {60, {{{"$", {LR::ActionType::Reduce, 24}}, {"fn", {LR::ActionType::Reduce, 24}}}, {}}},
            {61,
             {{{"identifier", {LR::ActionType::Reduce, 2}},
               {"if", {LR::ActionType::Reduce, 2}},
               {"let", {LR::ActionType::Reduce, 2}},
               {"return", {LR::ActionType::Reduce, 2}},
               {"}", {LR::ActionType::Reduce, 2}}},
              {}}},
            {62,
             {{{"!", {LR::ActionType::Shift, 32}},
               {"(", {LR::ActionType::Shift, 33}},
               {"-", {LR::ActionType::Shift, 34}},
               {"identifier", {LR::ActionType::Shift, 36}},
               {"literal", {LR::ActionType::Shift, 37}}},
              {{"<call_arguments>", 78},
               {"<expression>", 43},
               {"<factor>", 38},
               {"<function_call>", 35},
               {"<term>", 31}}}},
            {63,
             {{{"!", {LR::ActionType::Reduce, 26}},
               {")", {LR::ActionType::Reduce, 26}},
               {"*", {LR::ActionType::Reduce, 26}},
               {"+", {LR::ActionType::Reduce, 26}},
               {",", {LR::ActionType::Reduce, 26}},
               {"-", {LR::ActionType::Reduce, 26}},
               {"/", {LR::ActionType::Reduce, 26}},
               {";", {LR::ActionType::Reduce, 26}},
               {"<", {LR::ActionType::Reduce, 26}},
               {"=", {LR::ActionType::Reduce, 26}},
               {">", {LR::ActionType::Reduce, 26}},
               {"{", {LR::ActionType::Reduce, 26}}},
              {}}},
            {64,
             {{{"!", {LR::ActionType::Shift, 47}},
               {"+", {LR::ActionType::Shift, 48}},
               {"-", {LR::ActionType::Shift, 49}},
               {";", {LR::ActionType::Shift, 79}},
               {"<", {LR::ActionType::Shift, 50}},
               {"=", {LR::ActionType::Shift, 51}},
               {">", {LR::ActionType::Shift, 52}}},
              {}}},
            {65,
             {{{"!", {LR::ActionType::Shift, 32}},
               {"(", {LR::ActionType::Shift, 33}},
               {"-", {LR::ActionType::Shift, 34}},
               {"identifier", {LR::ActionType::Shift, 36}},
               {"literal", {LR::ActionType::Shift, 37}}},
              {{"<factor>", 38}, {"<function_call>", 35}, {"<term>", 80}}}},
            {66,
             {{{"!", {LR::ActionType::Reduce, 9}},
               {")", {LR::ActionType::Reduce, 9}},
               {"*", {LR::ActionType::Shift, 54}},
               {"+", {LR::ActionType::Reduce, 9}},
               {",", {LR::ActionType::Reduce, 9}},
               {"-", {LR::ActionType::Reduce, 9}},
               {"/", {LR::ActionType::Shift, 55}},
               {";", {LR::ActionType::Reduce, 9}},
               {"<", {LR::ActionType::Reduce, 9}},
               {"=", {LR::ActionType::Reduce, 9}},
               {">", {LR::ActionType::Reduce, 9}},
               {"{", {LR::ActionType::Reduce, 9}}},
              {}}},
            {67,
             {{{"!", {LR::ActionType::Reduce, 10}},
               {")", {LR::ActionType::Reduce, 10}},
               {"*", {LR::ActionType::Shift, 54}},
               {"+", {LR::ActionType::Reduce, 10}},
               {",", {LR::ActionType::Reduce, 10}},
               {"-", {LR::ActionType::Reduce, 10}},
               {"/", {LR::ActionType::Shift, 55}},
               {";", {LR::ActionType::Reduce, 10}},
               {"<", {LR::ActionType::Reduce, 10}},
               {"=", {LR::ActionType::Reduce, 10}},
               {">", {LR::ActionType::Reduce, 10}},
               {"{", {LR::ActionType::Reduce, 10}}},
              {}}},
            {68,
             {{{"!", {LR::ActionType::Reduce, 11}},
               {")", {LR::ActionType::Reduce, 11}},
               {"*", {LR::ActionType::Shift, 54}},
               {"+", {LR::ActionType::Reduce, 11}},
               {",", {LR::ActionType::Reduce, 11}},
               {"-", {LR::ActionType::Reduce, 11}},
               {"/", {LR::ActionType::Shift, 55}},
               {";", {LR::ActionType::Reduce, 11}},
               {"<", {LR::ActionType::Reduce, 11}},
               {"=", {LR::ActionType::Reduce, 11}},
               {">", {LR::ActionType::Reduce, 11}},
               {"{", {LR::ActionType::Reduce, 11}}},
              {}}},
            {69,
             {{{"!", {LR::ActionType::Shift, 32}},
               {"(", {LR::ActionType::Shift, 33}},
               {"-", {LR::ActionType::Shift, 34}},
               {"identifier", {LR::ActionType::Shift, 36}},
               {"literal", {LR::ActionType::Shift, 37}}},
              {{"<factor>", 38}, {"<function_call>", 35}, {"<term>", 81}}}},
            {70,
             {{{"!", {LR::ActionType::Shift, 32}},
               {"(", {LR::ActionType::Shift, 33}},
               {"-", {LR::ActionType::Shift, 34}},
               {"identifier", {LR::ActionType::Shift, 36}},
               {"literal", {LR::ActionType::Shift, 37}}},
              {{"<factor>", 38}, {"<function_call>", 35}, {"<term>", 82}}}},
            {71,
             {{{"!", {LR::ActionType::Reduce, 14}},
               {")", {LR::ActionType::Reduce, 14}},
               {"*", {LR::ActionType::Shift, 54}},
               {"+", {LR::ActionType::Reduce, 14}},
               {",", {LR::ActionType::Reduce, 14}},
               {"-", {LR::ActionType::Reduce, 14}},
               {"/", {LR::ActionType::Shift, 55}},
               {";", {LR::ActionType::Reduce, 14}},
               {"<", {LR::ActionType::Reduce, 14}},
               {"=", {LR::ActionType::Reduce, 14}},
               {">", {LR::ActionType::Reduce, 14}},
               {"{", {LR::ActionType::Reduce, 14}}},
              {}}},
            {72,
             {{{"!", {LR::ActionType::Shift, 32}},
               {"(", {LR::ActionType::Shift, 33}},
               {"-", {LR::ActionType::Shift, 34}},
               {"identifier", {LR::ActionType::Shift, 36}},
               {"literal", {LR::ActionType::Shift, 37}}},
              {{"<factor>", 38}, {"<function_call>", 35}, {"<term>", 83}}}},
            {73, {{{"}", {LR::ActionType::Shift, 84}}}, {}}},
            {74,
             {{{"!", {LR::ActionType::Reduce, 37}},
               {")", {LR::ActionType::Reduce, 37}},
               {"*", {LR::ActionType::Reduce, 37}},
               {"+", {LR::ActionType::Reduce, 37}},
               {",", {LR::ActionType::Reduce, 37}},
               {"-", {LR::ActionType::Reduce, 37}},
               {"/", {LR::ActionType::Reduce, 37}},
               {";", {LR::ActionType::Reduce, 37}},
               {"<", {LR::ActionType::Reduce, 37}},
               {"=", {LR::ActionType::Reduce, 37}},
               {">", {LR::ActionType::Reduce, 37}},
               {"{", {LR::ActionType::Reduce, 37}}},
              {}}},
            {75,
             {{{"!", {LR::ActionType::Reduce, 38}},
               {")", {LR::ActionType::Reduce, 38}},
               {"*", {LR::ActionType::Reduce, 38}},
               {"+", {LR::ActionType::Reduce, 38}},
               {",", {LR::ActionType::Reduce, 38}},
               {"-", {LR::ActionType::Reduce, 38}},
               {"/", {LR::ActionType::Reduce, 38}},
               {";", {LR::ActionType::Reduce, 38}},
               {"<", {LR::ActionType::Reduce, 38}},
               {"=", {LR::ActionType::Reduce, 38}},
               {">", {LR::ActionType::Reduce, 38}},
               {"{", {LR::ActionType::Reduce, 38}}},
              {}}},
            {76,
             {{{"!", {LR::ActionType::Shift, 47}},
               {")", {LR::ActionType::Shift, 85}},
               {"+", {LR::ActionType::Shift, 48}},
               {"-", {LR::ActionType::Shift, 49}},
               {"<", {LR::ActionType::Shift, 50}},
               {"=", {LR::ActionType::Shift, 51}},
               {">", {LR::ActionType::Shift, 52}}},
              {}}},
            {77,
             {{{"!", {LR::ActionType::Reduce, 18}},
               {")", {LR::ActionType::Reduce, 18}},
               {"*", {LR::ActionType::Reduce, 18}},
               {"+", {LR::ActionType::Reduce, 18}},
               {",", {LR::ActionType::Reduce, 18}},
               {"-", {LR::ActionType::Reduce, 18}},
               {"/", {LR::ActionType::Reduce, 18}},
               {";", {LR::ActionType::Reduce, 18}},
               {"<", {LR::ActionType::Reduce, 18}},
               {"=", {LR::ActionType::Reduce, 18}},
               {">", {LR::ActionType::Reduce, 18}},
               {"{", {LR::ActionType::Reduce, 18}}},
              {}}},
            {78, {{{")", {LR::ActionType::Reduce, 7}}}, {}}},
            {79,
             {{{"identifier", {LR::ActionType::Reduce, 3}},
               {"if", {LR::ActionType::Reduce, 3}},
               {"let", {LR::ActionType::Reduce, 3}},
               {"return", {LR::ActionType::Reduce, 3}},
               {"}", {LR::ActionType::Reduce, 3}}},
              {}}},
            {80,
             {{{"!", {LR::ActionType::Reduce, 8}},
               {")", {LR::ActionType::Reduce, 8}},
               {"*", {LR::ActionType::Shift, 54}},
               {"+", {LR::ActionType::Reduce, 8}},
               {",", {LR::ActionType::Reduce, 8}},
               {"-", {LR::ActionType::Reduce, 8}},
               {"/", {LR::ActionType::Shift, 55}},
               {";", {LR::ActionType::Reduce, 8}},
               {"<", {LR::ActionType::Reduce, 8}},
               {"=", {LR::ActionType::Reduce, 8}},
               {">", {LR::ActionType::Reduce, 8}},
               {"{", {LR::ActionType::Reduce, 8}}},
              {}}},
            {81,
             {{{"!", {LR::ActionType::Reduce, 12}},
               {")", {LR::ActionType::Reduce, 12}},
               {"*", {LR::ActionType::Shift, 54}},
               {"+", {LR::ActionType::Reduce, 12}},
               {",", {LR::ActionType::Reduce, 12}},
               {"-", {LR::ActionType::Reduce, 12}},
               {"/", {LR::ActionType::Shift, 55}},
               {";", {LR::ActionType::Reduce, 12}},
               {"<", {LR::ActionType::Reduce, 12}},
               {"=", {LR::ActionType::Reduce, 12}},
               {">", {LR::ActionType::Reduce, 12}},
               {"{", {LR::ActionType::Reduce, 12}}},
              {}}},
            {82,
             {{{"!", {LR::ActionType::Reduce, 13}},
               {")", {LR::ActionType::Reduce, 13}},
               {"*", {LR::ActionType::Shift, 54}},
               {"+", {LR::ActionType::Reduce, 13}},
               {",", {LR::ActionType::Reduce, 13}},
               {"-", {LR::ActionType::Reduce, 13}},
               {"/", {LR::ActionType::Shift, 55}},
               {";", {LR::ActionType::Reduce, 13}},
               {"<", {LR::ActionType::Reduce, 13}},
               {"=", {LR::ActionType::Reduce, 13}},
               {">", {LR::ActionType::Reduce, 13}},
               {"{", {LR::ActionType::Reduce, 13}}},
              {}}},
            {83,
             {{{"!", {LR::ActionType::Reduce, 15}},
               {")", {LR::ActionType::Reduce, 15}},
               {"*", {LR::ActionType::Shift, 54}},
               {"+", {LR::ActionType::Reduce, 15}},
               {",", {LR::ActionType::Reduce, 15}},
               {"-", {LR::ActionType::Reduce, 15}},
               {"/", {LR::ActionType::Shift, 55}},
               {";", {LR::ActionType::Reduce, 15}},
               {"<", {LR::ActionType::Reduce, 15}},
               {"=", {LR::ActionType::Reduce, 15}},
               {">", {LR::ActionType::Reduce, 15}},
               {"{", {LR::ActionType::Reduce, 15}}},
              {}}},
            {84,
             {{{"else", {LR::ActionType::Shift, 86}},
               {"identifier", {LR::ActionType::Reduce, 27}},
               {"if", {LR::ActionType::Reduce, 27}},
               {"let", {LR::ActionType::Reduce, 27}},
               {"return", {LR::ActionType::Reduce, 27}},
               {"}", {LR::ActionType::Reduce, 27}}},
              {}}},
            {85,
             {{{"!", {LR::ActionType::Reduce, 17}},
               {")", {LR::ActionType::Reduce, 17}},
               {"*", {LR::ActionType::Reduce, 17}},
               {"+", {LR::ActionType::Reduce, 17}},
               {",", {LR::ActionType::Reduce, 17}},
               {"-", {LR::ActionType::Reduce, 17}},
               {"/", {LR::ActionType::Reduce, 17}},
               {";", {LR::ActionType::Reduce, 17}},
               {"<", {LR::ActionType::Reduce, 17}},
               {"=", {LR::ActionType::Reduce, 17}},
               {">", {LR::ActionType::Reduce, 17}},
               {"{", {LR::ActionType::Reduce, 17}}},
              {}}},
            {86, {{{"{", {LR::ActionType::Shift, 87}}}, {}}},
            {87,
             {{{"identifier", {LR::ActionType::Shift, 14}},
               {"if", {LR::ActionType::Shift, 18}},
               {"let", {LR::ActionType::Shift, 15}},
               {"return", {LR::ActionType::Shift, 19}}},
              {{"<assignment>", 20},
               {"<body>", 88},
               {"<function_call>", 21},
               {"<if>", 22},
               {"<return>", 23},
               {"<statement>", 16}}}},
            {88, {{{"}", {LR::ActionType::Shift, 89}}}, {}}},
            {89,
             {{{"identifier", {LR::ActionType::Reduce, 28}},
               {"if", {LR::ActionType::Reduce, 28}},
               {"let", {LR::ActionType::Reduce, 28}},
               {"return", {LR::ActionType::Reduce, 28}},
               {"}", {LR::ActionType::Reduce, 28}}},
              {}}}};

        CHECK_EQ(table, get_table());
    }
}
