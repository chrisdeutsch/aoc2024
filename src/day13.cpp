#include <iostream>
#include <print>
#include <regex>
#include <sstream>

struct vec2u {
    std::size_t x;
    std::size_t y;
};

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

long det(long a, long b, long c, long d) { return a * d - b * c; }

std::optional<std::size_t> solve_game_min_cost(const game &g) {
    const auto D = det(g.button_a.x, g.button_b.x, g.button_a.y, g.button_b.y);
    if (D == 0) {
        // No solution
        return {};
    }

    const auto D_a = det(g.prize.x, g.button_b.x, g.prize.y, g.button_b.y);
    if (D_a % D != 0) {
        // No integer solution
        return {};
    }

    const auto D_b = det(g.button_a.x, g.prize.x, g.button_a.y, g.prize.y);
    if (D_b % D != 0) {
        // No integer solution
        return {};
    }

    const auto a = D_a / D;
    const auto b = D_b / D;
    return 3 * a + b;
}

int main() {
    const auto games = read_input(std::cin);

    std::size_t total_pt1 = 0;
    std::size_t total_pt2 = 0;
    for (const auto &g : games) {
        const auto min_cost = solve_game_min_cost(g);
        if (min_cost) {
            total_pt1 += *min_cost;
        }

        game game_pt2 = g;
        game_pt2.prize.x += 10000000000000uz;
        game_pt2.prize.y += 10000000000000uz;
        const auto min_cost_pt2 = solve_game_min_cost(game_pt2);
        if (min_cost_pt2) {
            total_pt2 += *min_cost_pt2;
        }
    }

    std::println("Part 1: {}", total_pt1);
    std::println("Part 2: {}", total_pt2);

    return 0;
}
