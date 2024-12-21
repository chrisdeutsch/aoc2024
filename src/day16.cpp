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
    std::vector<position> pos_history;

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

void push_path_to_heap(const auto &candidate_path, orientation orient, std::vector<path> &paths) {
    const auto number_of_turns = get_number_of_turns(candidate_path.orient, orient);
    const auto new_cost = candidate_path.cost + 1000 * number_of_turns + 1;

    auto [row, col] = candidate_path.pos;
    switch (orient) {
    case orientation::north:
        row -= 1;
        break;
    case orientation::east:
        col += 1;
        break;
    case orientation::south:
        row += 1;
        break;
    case orientation::west:
        col -= 1;
        break;
    }

    auto pos_history = candidate_path.pos_history;
    pos_history.emplace_back(candidate_path.pos);

    paths.emplace_back(path{.pos = {.row = row, .col = col},
                            .orient = orient,
                            .cost = new_cost,
                            .pos_history = std::move(pos_history)});
    std::push_heap(paths.begin(), paths.end(), std::greater<>());
}

std::vector<path> solve_maze(const maze &m) {
    std::vector<path> best_paths;
    std::optional<std::size_t> best_cost;

    std::vector<path> paths;
    paths.emplace_back(path{.pos = m.start, .orient = orientation::east, .cost = 0});
    std::push_heap(paths.begin(), paths.end(), std::greater<>());

    std::map<std::pair<position, orientation>, std::size_t> min_cost_by_location;

    while (true) {
        std::pop_heap(paths.begin(), paths.end(), std::greater<>());
        const auto candidate_path = paths.back();
        paths.pop_back();

        if (best_cost && candidate_path.cost > *best_cost) {
            break;
        }

        if (candidate_path.pos == m.end) {
            best_paths.push_back(candidate_path);
            best_cost = candidate_path.cost;
            continue;
        }

        if (auto it = min_cost_by_location.find({candidate_path.pos, candidate_path.orient});
            it != min_cost_by_location.end() && it->second < candidate_path.cost) {
            continue;
        } else {
            min_cost_by_location[{candidate_path.pos, candidate_path.orient}] = candidate_path.cost;
        }

        const auto [row, col] = candidate_path.pos;

        // We disallow backtracking / 180 degree turns as this will always incur a higher cost
        // North
        if (candidate_path.orient != orientation::south && !m.walls.contains({row - 1, col})) {
            push_path_to_heap(candidate_path, orientation::north, paths);
        }
        // East
        if (candidate_path.orient != orientation::west && !m.walls.contains({row, col + 1})) {
            push_path_to_heap(candidate_path, orientation::east, paths);
        }
        // South
        if (candidate_path.orient != orientation::north && !m.walls.contains({row + 1, col})) {
            push_path_to_heap(candidate_path, orientation::south, paths);
        }
        // West
        if (candidate_path.orient != orientation::east && !m.walls.contains({row, col - 1})) {
            push_path_to_heap(candidate_path, orientation::west, paths);
        }
    }

    return best_paths;
}

void print_vec(std::vector<path> vec) {
    for (const auto &elem : vec) {
        std::print("{}, ", elem.cost);
    }
    std::println("");
}

int main() {
    const auto m = read_input(std::cin);
    const auto best_paths = solve_maze(m);

    std::println("Part 1: {}", best_paths.front().cost);

    // TODO: Could use backtracking instead of brute force for part 2
    std::set<position> visited_locations;
    for (const auto &path : best_paths) {
        for (const auto loc : path.pos_history) {
            visited_locations.insert(loc);
        }
    }

    // Add one because we have to count the end of the maze (which is not part of the path history)
    std::println("Part 2: {}", visited_locations.size() + 1);

    return 0;
}
