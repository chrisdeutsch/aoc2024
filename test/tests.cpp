#include "parsing.h"
#include <catch2/catch_test_macros.hpp>
#include <sstream>

TEST_CASE("test_parse_input", "[parsing]") {
    std::istringstream ss("1 2\n3 4\n5 6");
    const auto result = parse_input<int, int>(ss);

    const std::vector<std::tuple<int, int>> expected = {
        {1, 2},
        {3, 4},
        {5, 6},
    };

    REQUIRE(result == expected);
}

TEST_CASE("test_parse_input_newl_at_eof", "[parsing]") {
    std::istringstream ss("1 2\n3 4\n5 6\n");
    const auto result = parse_input<int, int>(ss);

    const std::vector<std::tuple<int, int>> expected = {
        {1, 2},
        {3, 4},
        {5, 6},
    };

    REQUIRE(result == expected);
}

TEST_CASE("test_parse_input_incomplete_line", "[parsing]") {
    std::istringstream ss("1 2\n3\n5 6");
    REQUIRE_THROWS(parse_input<int, int>(ss));
}
