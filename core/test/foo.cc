#include "doctest/doctest.h"

#include "../src/foo.h"

TEST_SUITE("foo") {
    TEST_CASE("returns_one") { CHECK_EQ(foo(), 1); }
}
