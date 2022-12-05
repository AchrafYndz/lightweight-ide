#include "doctest/doctest.h"

#include "../src/parser/lr.h"

class LRTest {
protected:
    LR::ItemSet closure(const LR::ItemSet& item, const std::string& separator, const CFG& cfg) {
        return LR::closure(item, separator, cfg);
    }
};

TEST_SUITE("LRTests") {
    TEST_CASE_FIXTURE(LRTest, "[LRTests] closure") {
        const std::string separator("·");
        const LR::ItemSet start_item_set{{LR::Item{"S", {separator, "A", "A"}}}};

        const CFG cfg("./test/res/input/CFG2.json");

        CHECK_EQ(closure(start_item_set, separator, cfg),
                 LR::ItemSet{{{"A", {"·", "a", "A"}}, {"A", {"·", "b"}}, {"S", {"·", "A", "A"}}}});
    }
}
