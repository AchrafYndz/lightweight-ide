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

    TEST_CASE_FIXTURE(LRTest, "[LRTest] parsing_table") {
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
            {6,
             {{{"$", {LR::ActionType::Reduce, 2}},
               {"a", {LR::ActionType::Reduce, 2}},
               {"b", {LR::ActionType::Reduce, 2}}},
              {}}}};

        CHECK_EQ(table, get_table());
    }
}
