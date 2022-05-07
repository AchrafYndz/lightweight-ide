#include "../../lib/doctest/doctest.h"

TEST_SUITE("Test") {
    TEST_CASE("foo") { CHECK(true); }
}

TEST_SUITE("Test2") {
    TEST_CASE("foo2") { CHECK(false); }
}