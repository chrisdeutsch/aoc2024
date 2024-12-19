#include <iostream>
#include <print>
#include <regex>
#include <sstream>

struct vec2u {
    std::size_t x;
    std::size_t y;

    constexpr vec2u operator+(const vec2u &other) const {
        return {.x = x + other.x, .y = y + other.y};
    }

    constexpr vec2u operator-(const vec2u &other) const {
        return {.x = x - other.x, .y = y - other.y};
    }

    constexpr vec2u operator*(std::size_t scalar) const { return {scalar * x, scalar * y}; }

    constexpr bool operator==(const vec2u &other) const { return x == other.x && y == other.y; }
};

constexpr vec2u operator*(std::size_t scalar, const vec2u &vec) { return vec * scalar; }

struct game {
    vec2u button_a;
    vec2u button_b;
    vec2u prize;
};

std::vector<game> read_input(std::istream &istream) {
    std::stringstream ss;
    ss << istream.rdbuf();
    const auto content = ss.str();

    const std::regex pattern("Button A: X\\+(\\d+), Y\\+(\\d+)\\nButton B: X\\+(\\d+), "
                             "Y\\+(\\d+)\\nPrize: X=(\\d+), Y=(\\d+)");

    std::vector<game> games;
    auto begin = std::sregex_iterator(content.cbegin(), content.cend(), pattern);
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it) {
        std::smatch match = *it;

        auto button_a = vec2u{.x = std::stoul(match[1]), .y = std::stoul(match[2])};
        auto button_b = vec2u{.x = std::stoul(match[3]), .y = std::stoul(match[4])};
        auto prize = vec2u{.x = std::stoul(match[5]), .y = std::stoul(match[6])};

        games.emplace_back(game{.button_a = std::move(button_a),
                                .button_b = std::move(button_b),
                                .prize = std::move(prize)});
    }

    return games;
}

std::optional<std::size_t> solve_game_min_cost(const game &g) {
    std::size_t min_cost = std::numeric_limits<std::size_t>::max();
    std::size_t na = 0;
    while (true) {
        const auto a = na * g.button_a;
        if (g.prize.x < a.x || g.prize.y < a.y) {
            // No more solutions
            break;
        }

        // We need b = nb * vecb = prize
        const auto delta = g.prize - a;
        const auto nb = delta.x / g.button_b.x;
        if (nb * g.button_b == delta) {
            const auto cost = 3 * na + nb;
            min_cost = std::min(min_cost, cost);
        }
        ++na;
    }

    if (min_cost == std::numeric_limits<std::size_t>::max()) {
        return {};
    }

    return min_cost;
}

int main() {
    const auto games = read_input(std::cin);

    std::size_t total = 0;
    for (const auto &game : games) {
        const auto min_cost = solve_game_min_cost(game);
        if (min_cost) {
            total += *min_cost;
        }
    }

    std::println("Part 1: {}", total);

    // ILP (Integer Linear Programming)?
    //
    // minimize cost(n_a, n_b) = n_a * 3 + n_b * 1
    //
    // subject to
    //
    // prize = n_a * da + n_b * db
    // -> prize_x = n_a * da_x + n_b * db_x
    // -> prize_y = n_a * da_y + n_b * db_y

    return 0;
}
