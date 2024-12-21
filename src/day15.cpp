#include <algorithm>
#include <iostream>
#include <print>
#include <ranges>
#include <set>
#include <string>
#include <utility>
#include <vector>

enum class tile_type {
    wall = '#',
    empty = '.',
    box = 'O',
    robot = '@',
    big_box_left = '[',
    big_box_right = ']',
};

constexpr bool is_box(tile_type t) {
    return t == tile_type::box || t == tile_type::big_box_left || t == tile_type::big_box_right;
}

std::pair<tile_type, tile_type> to_wide_tile(tile_type t) {
    switch (t) {
    case tile_type::wall:
        return {tile_type::wall, tile_type::wall};
    case tile_type::box:
        return {tile_type::big_box_left, tile_type::big_box_right};
    case tile_type::empty:
        return {tile_type::empty, tile_type::empty};
    case tile_type::robot:
        return {tile_type::robot, tile_type::empty};
    case tile_type::big_box_left:
        break;
    case tile_type::big_box_right:
        break;
    }
    throw std::runtime_error("cannot convert tile to wide tile");
}

enum class move_direction {
    north = '^',
    east = '>',
    south = 'v',
    west = '<',
};

using warehouse = std::vector<std::vector<tile_type>>;

warehouse read_warehouse(std::istream &istream) {
    warehouse g;

    std::string line;
    while (std::getline(istream, line) && line != "") {
        auto row = line |
                   std::views::transform([](char ch) { return static_cast<tile_type>(ch); }) |
                   std::ranges::to<std::vector>();
        g.emplace_back(std::move(row));
    }
    return g;
}

warehouse to_wide_warehouse(const warehouse &g) {
    warehouse new_wh;
    new_wh.reserve(g.size());

    for (const auto &row : g) {
        std::vector<tile_type> new_row;
        new_row.reserve(2 * row.size());

        for (const auto t : row) {
            const auto [left, right] = to_wide_tile(t);
            new_row.push_back(left);
            new_row.push_back(right);
        }

        new_wh.emplace_back(std::move(new_row));
    }

    return new_wh;
}

std::vector<move_direction> read_moves(std::istream &istream) {
    std::vector<move_direction> moves;

    std::string line;
    while (std::getline(istream, line)) {
        std::transform(line.cbegin(), line.cend(), std::back_inserter(moves),
                       [](char ch) { return static_cast<move_direction>(ch); });
    }
    return moves;
}

std::pair<std::size_t, std::size_t> find_robot(const warehouse &wh) {
    for (std::size_t row = 0; row < wh.size(); ++row) {
        for (std::size_t col = 0; col < wh[row].size(); ++col) {
            if (wh[row][col] == tile_type::robot) {
                return {row, col};
            }
        }
    }
    throw std::runtime_error("did not find robot");
}

constexpr std::pair<int, int> move_to_direction(move_direction m) {
    switch (m) {
    case move_direction::north:
        return {-1, 0};
    case move_direction::east:
        return {0, 1};
    case move_direction::south:
        return {1, 0};
    case move_direction::west:
        return {0, -1};
    }
    std::unreachable();
}

bool move_box(std::size_t row, std::size_t col, move_direction m, warehouse &wh) {
    using coordinates = std::pair<std::size_t, std::size_t>;
    const auto dir = move_to_direction(m);

    std::set<coordinates> visited;
    std::vector<coordinates> boxes;
    std::vector<coordinates> stack;
    stack.emplace_back(coordinates{row, col});
    while (!stack.empty()) {
        const auto coords = stack.back();
        const auto [box_row, box_col] = coords;
        stack.pop_back();

        if (visited.contains(coords)) {
            continue;
        }
        visited.insert(coords);

        const auto t = wh[box_row][box_col];

        if (t == tile_type::wall || t == tile_type::robot) {
            return false; // not movable
        } else if (t == tile_type::empty) {
            continue;
        }
        // Otherwise: must be a box, big_box_left, or big_box_right
        boxes.push_back({box_row, box_col});

        const auto next_tile = wh[box_row + dir.first][box_col + dir.second];
        stack.emplace_back(box_row + dir.first, box_col + dir.second);

        // If we are moving east or west the matching box pair will have been added above
        if (m == move_direction::north || m == move_direction::south) {
            if (t == tile_type::big_box_left) {
                stack.push_back({box_row, box_col + 1});
                stack.push_back({box_row + dir.first, box_col + dir.second + 1});
            } else if (t == tile_type::big_box_right) {
                stack.push_back({box_row, box_col - 1});
                stack.push_back({box_row + dir.first, box_col + dir.second - 1});
            }
        }
    }

    // Perform the move
    warehouse new_wh = wh;

    for (const auto [row, col] : boxes) {
        new_wh[row][col] = tile_type::empty;
    }
    for (const auto [row, col] : boxes) {
        new_wh[row + dir.first][col + dir.second] = wh[row][col];
    }

    wh = std::move(new_wh);
    return true;
}

void simulate(warehouse &wh, const std::vector<move_direction> &moves) {
    auto [robot_row, robot_col] = find_robot(wh);

    for (const auto m : moves) {
        const auto direction = move_to_direction(m);

        std::size_t robot_dest_row = robot_row + direction.first;
        std::size_t robot_dest_col = robot_col + direction.second;

        const auto dest_tile = wh[robot_dest_row][robot_dest_col];
        if (dest_tile == tile_type::wall) {
            // Facing wall: do nothing
        } else if (dest_tile == tile_type::empty) {
            // Empty space: move one step
            wh[robot_row][robot_col] = tile_type::empty;
            wh[robot_dest_row][robot_dest_col] = tile_type::robot;
            robot_row = robot_dest_row;
            robot_col = robot_dest_col;
        } else if (is_box(dest_tile)) {
            if (move_box(robot_dest_row, robot_dest_col, m, wh)) {
                // Boxes were moved -> move the robot
                wh[robot_row][robot_col] = tile_type::empty;
                wh[robot_dest_row][robot_dest_col] = tile_type::robot;
                robot_row = robot_dest_row;
                robot_col = robot_dest_col;
            }
        }
    }
}

std::size_t sum_of_box_gps_coordinates(const warehouse &wh) {
    std::size_t total = 0;
    for (const auto &[row_idx, row] : wh | std::views::enumerate) {
        for (const auto &[col_idx, tile] : row | std::views::enumerate) {
            if (tile == tile_type::box || tile == tile_type::big_box_left) {
                total += 100 * row_idx + col_idx;
            }
        }
    }
    return total;
}

void print_grid(const warehouse &grid) {
    for (const auto &row : grid) {
        for (const auto tile : row) {
            std::print("{}", static_cast<char>(tile));
        }
        std::print("\n");
    }
}

int main() {
    const auto wh = read_warehouse(std::cin);
    const auto moves = read_moves(std::cin);

    auto wh_clone = wh;
    simulate(wh_clone, moves);

    std::println("Part 1: {}", sum_of_box_gps_coordinates(wh_clone));

    auto wh_wide = to_wide_warehouse(wh);
    simulate(wh_wide, moves);

    std::println("Part 2: {}", sum_of_box_gps_coordinates(wh_wide));
    return 0;
}
