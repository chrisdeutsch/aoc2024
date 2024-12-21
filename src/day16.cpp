#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <optional>
#include <print>
#include <ranges>
#include <set>
#include <string>
#include <vector>

struct position {
    std::size_t row;
    std::size_t col;
    constexpr auto operator<=>(const position &other) const = default;
};

struct maze {
    position start;
    position end;
    std::set<position> walls;
};

enum class orientation {
    north = 0,
    east = 1,
    south = 2,
    west = 3,
};

struct path {
    position pos;
    orientation orient;
    std::size_t cost;

    constexpr bool operator<(const path &other) const { return cost < other.cost; }

    constexpr bool operator>(const path &other) const { return cost > other.cost; }
};

[[nodiscard]]
std::size_t get_number_of_turns(orientation from, orientation to) {
    const auto from_int = static_cast<int>(from);
    const auto to_int = static_cast<int>(to);
    const auto dist = std::abs(from_int - to_int);
    return std::min(dist, 4 - dist);
}

maze read_input(std::istream &is) {
    std::set<position> walls;
    std::optional<position> start;
    std::optional<position> end;

    std::string line;
    auto row = 0uz;
    while (std::getline(is, line)) {
        for (const auto [col, ch] : line | std::views::enumerate) {
            switch (ch) {
            case '#':
                walls.emplace(row, col);
                break;
            case 'S':
                start = {row, static_cast<std::size_t>(col)};
                break;
            case 'E':
                end = {row, static_cast<std::size_t>(col)};
                break;
            case '.':
                break;
            default:
                throw std::runtime_error(std::format("Unknown input '{}'", ch));
            }
        }
        ++row;
    }

    if (!start) {
        throw std::runtime_error("maze start not found");
    }
    if (!end) {
        throw std::runtime_error("maze end not found");
    }

    return {*start, *end, std::move(walls)};
}

void print_maze(const auto &m) {
    const auto num_rows =
        std::ranges::max(m.walls | std::views::transform([](const auto &p) { return p.row; })) + 1;
    const auto num_cols =
        std::ranges::max(m.walls | std::views::transform([](const auto &p) { return p.col; })) + 1;

    for (std::size_t row = 0; row < num_rows; ++row) {
        for (std::size_t col = 0; col < num_cols; ++col) {
            if (m.walls.contains({row, col})) {
                std::print("#");
            } else if (position{row, col} == m.start) {
                std::print("S");
            } else if (position{row, col} == m.end) {
                std::print("E");
            } else {
                std::print(".");
            }
        }
        std::println("");
    }
}

std::size_t solve_maze(const maze &m) {
    std::vector<path> paths;
    paths.emplace_back(path{.pos = m.start, .orient = orientation::east, .cost = 0});
    std::push_heap(paths.begin(), paths.end(), std::greater<>());

    std::map<std::pair<position, orientation>, std::size_t> min_cost_by_location;

    while (true) {
        std::pop_heap(paths.begin(), paths.end(), std::greater<>());
        const auto candidate_path = paths.back();
        paths.pop_back();

        if (candidate_path.pos == m.end) {
            return candidate_path.cost;
        }

        if (auto it = min_cost_by_location.find({candidate_path.pos, candidate_path.orient});
            it != min_cost_by_location.end() && it->second < candidate_path.cost) {
            continue;
        } else {
            min_cost_by_location[{candidate_path.pos, candidate_path.orient}] = candidate_path.cost;
        }

        const auto [row, col] = candidate_path.pos;

        // North
        if (!m.walls.contains({row - 1, col})) {
            const auto number_of_turns =
                get_number_of_turns(candidate_path.orient, orientation::north);
            const auto new_cost = candidate_path.cost + 1000 * number_of_turns + 1;

            paths.emplace_back(path{.pos = {.row = row - 1, .col = col},
                                    .orient = orientation::north,
                                    .cost = new_cost});
            std::push_heap(paths.begin(), paths.end(), std::greater<>());
        }
        // East
        if (!m.walls.contains({row, col + 1})) {
            const auto number_of_turns =
                get_number_of_turns(candidate_path.orient, orientation::east);
            const auto new_cost = candidate_path.cost + 1000 * number_of_turns + 1;

            paths.emplace_back(path{.pos = {.row = row, .col = col + 1},
                                    .orient = orientation::east,
                                    .cost = new_cost});
            std::push_heap(paths.begin(), paths.end(), std::greater<>());
        }
        // South
        if (!m.walls.contains({row + 1, col})) {
            const auto number_of_turns =
                get_number_of_turns(candidate_path.orient, orientation::south);
            const auto new_cost = candidate_path.cost + 1000 * number_of_turns + 1;

            paths.emplace_back(path{.pos = {.row = row + 1, .col = col},
                                    .orient = orientation::south,
                                    .cost = new_cost});
            std::push_heap(paths.begin(), paths.end(), std::greater<>());
        }
        // West
        if (!m.walls.contains({row, col - 1})) {
            const auto number_of_turns =
                get_number_of_turns(candidate_path.orient, orientation::west);
            const auto new_cost = candidate_path.cost + 1000 * number_of_turns + 1;

            paths.emplace_back(path{.pos = {.row = row, .col = col - 1},
                                    .orient = orientation::west,
                                    .cost = new_cost});
            std::push_heap(paths.begin(), paths.end(), std::greater<>());
        }
    }
}

void print_vec(std::vector<path> vec) {
    for (const auto &elem : vec) {
        std::print("{}, ", elem.cost);
    }
    std::println("");
}

int main() {
    const auto m = read_input(std::cin);
    print_maze(m);

    const auto min_cost = solve_maze(m);
    std::println("min cost: {}", min_cost);

    return 0;
}
