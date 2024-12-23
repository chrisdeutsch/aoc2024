#include <absl/strings/str_split.h>
#include <cmath>
#include <iostream>
#include <print>
#include <queue>
#include <set>
#include <string>

struct position {
    std::size_t row;
    std::size_t col;
    constexpr auto operator<=>(const position &other) const = default;
};

struct pathfinding_state {
    position pos;
    std::size_t steps;
    std::size_t steps_lower_bound;
    bool operator>(const pathfinding_state &other) const {
        return steps_lower_bound > other.steps_lower_bound;
    }
};

struct maze {
    std::size_t width;
    std::size_t height;
    std::set<position> walls;
    position start;
    position end;
};

std::vector<position> read_input(std::istream &is) {
    std::vector<position> corrupted_bytes;
    std::string line;
    while (std::getline(is, line)) {
        const std::pair<std::string, std::string> coords = absl::StrSplit(line, ",");
        corrupted_bytes.emplace_back(std::stoul(coords.second), std::stoul(coords.first));
    }
    return corrupted_bytes;
}

void print_maze(std::size_t width, std::size_t height, const std::set<position> &corrupted_bytes) {
    for (std::size_t row = 0; row < height; ++row) {
        for (std::size_t col = 0; col < width; ++col) {
            if (corrupted_bytes.contains({row, col})) {
                std::print("#");
            } else {
                std::print(".");
            }
        }
        std::print("\n");
    }
}

std::size_t manhatten_distance(position pos1, position pos2) {
    const std::size_t delta_row = pos1.row > pos2.row ? pos1.row - pos2.row : pos2.row - pos1.row;
    const std::size_t delta_col = pos1.col > pos2.col ? pos1.col - pos2.col : pos2.col - pos1.col;
    return delta_row + delta_col;
}

std::optional<position> move(position start, int delta_row, int delta_col, std::size_t num_rows,
                             std::size_t num_cols) {
    const auto dest_row = static_cast<int>(start.row) + delta_row;
    const auto dest_col = static_cast<int>(start.col) + delta_col;
    if (0 <= dest_row && dest_row < num_rows && 0 <= dest_col && dest_col < num_cols) {
        return position{.row = static_cast<std::size_t>(dest_row),
                        .col = static_cast<std::size_t>(dest_col)};
    }
    return {};
}

std::optional<std::size_t> shortest_path(const maze &m) {
    std::optional<std::size_t> shortest_so_far;
    std::set<position> visited;
    std::priority_queue<pathfinding_state, std::vector<pathfinding_state>,
                        std::greater<pathfinding_state>>
        queue;
    queue.push({m.start, 0, manhatten_distance(m.start, m.end)});

    while (!queue.empty() &&
           (!shortest_so_far || queue.top().steps_lower_bound < *shortest_so_far)) {
        const auto [pos, steps, steps_lower_bound] = queue.top();
        queue.pop();
        visited.insert(pos);

        if (pos == m.end) {
            shortest_so_far = steps;
        }

        static const std::array<std::pair<int, int>, 4> directions = {
            std::pair<int, int>{-1, 0}, // North
            std::pair<int, int>{0, 1},  // East
            std::pair<int, int>{1, 0},  // South
            std::pair<int, int>{0, -1}, // West
        };

        for (const auto [delta_row, delta_col] : directions) {
            if (auto dest_pos = move(pos, delta_row, delta_col, m.height, m.width);
                dest_pos && !visited.contains(*dest_pos) && !m.walls.contains(*dest_pos)) {
                const auto lower_bound = steps + 1 + manhatten_distance(*dest_pos, m.end);
                queue.push({*dest_pos, steps + 1, lower_bound});
            }
        }
    }
    return shortest_so_far;
}

int main() {
    const auto corrupted_bytes = read_input(std::cin);
    std::set<position> walls(corrupted_bytes.cbegin(), corrupted_bytes.cbegin() + 1024);

    maze m = {
        .width = 71,
        .height = 71,
        .walls = std::move(walls),
        .start = {0, 0},
        .end = {70, 70},
    };

    const auto out = shortest_path(m);
    if (out) {
        std::println("Part 1: {}", *out);
    }

    // Part 2 (using binary search)
    std::size_t min = 1024;
    std::size_t max = corrupted_bytes.size() - 1;
    while (min + 1 < max) {
        const auto midpoint = (max + min) / 2;
        m.walls = std::set<position>(corrupted_bytes.cbegin(), corrupted_bytes.cbegin() + midpoint);
        if (shortest_path(m)) {
            min = midpoint;
        } else {
            max = midpoint;
        }
    }
    std::println("Part 2: {},{}", corrupted_bytes[max - 1].col, corrupted_bytes[max - 1].row);

    return 0;
}
