#include <algorithm>
#include <iostream>
#include <print>
#include <set>
#include <string>
#include <vector>

enum class tile_type {
    wall = '#',
    empty = '.',
    box = 'O',
    robot = '@',
    big_box_left = '[',
    big_box_right = ']',
};

enum class move {
    north = '^',
    east = '>',
    south = 'v',
    west = '<',
};

using grid = std::vector<std::vector<tile_type>>;

grid read_grid(std::istream &istream) {
    grid g;

    std::string line;
    while (std::getline(istream, line) && line != "") {
        std::vector<tile_type> row;
        row.reserve(line.size());
        std::transform(line.cbegin(), line.cend(), std::back_inserter(row),
                       [](char ch) { return static_cast<tile_type>(ch); });
        g.emplace_back(std::move(row));
    }
    return g;
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

grid convert_to_wide_grid(const grid &g) {
    grid new_grid;
    new_grid.reserve(g.size());

    for (const auto &row : g) {
        std::vector<tile_type> new_row;
        new_row.reserve(2 * row.size());

        for (const auto t : row) {
            const auto [left, right] = to_wide_tile(t);
            new_row.push_back(left);
            new_row.push_back(right);
        }

        new_grid.emplace_back(std::move(new_row));
    }

    return new_grid;
}

std::vector<move> read_moves(std::istream &istream) {
    std::vector<move> moves;

    std::string line;
    while (std::getline(istream, line)) {
        std::transform(line.cbegin(), line.cend(), std::back_inserter(moves),
                       [](char ch) { return static_cast<move>(ch); });
    }
    return moves;
}

std::pair<std::size_t, std::size_t> find_robot(const grid &grid) {
    for (std::size_t row = 0; row < grid.size(); ++row) {
        for (std::size_t col = 0; col < grid[row].size(); ++col) {
            if (grid[row][col] == tile_type::robot) {
                return {row, col};
            }
        }
    }
    throw std::runtime_error("did not find robot");
}

std::pair<int, int> move_to_direction(move m) {
    switch (m) {
    case move::north:
        return {-1, 0};
    case move::east:
        return {0, 1};
    case move::south:
        return {1, 0};
    case move::west:
        return {0, -1};
    }
    throw std::runtime_error("Cannot convert move to direction");
}

bool move_box(std::size_t row, std::size_t col, move m, grid &g) {
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

        const auto t = g[box_row][box_col];
        switch (t) {
        case tile_type::wall:
            return false; // not movable
        case tile_type::robot:
            return false; // not movable
        case tile_type::empty:
            continue;
        case tile_type::box:
            break;
        case tile_type::big_box_left:
            break;
        case tile_type::big_box_right:
            break;
        }
        boxes.push_back({box_row, box_col});

        const auto next_tile = g[box_row + dir.first][box_col + dir.second];
        stack.push_back({box_row + dir.first, box_col + dir.second});

        // If we are moving east or west the matching box pair will have been added above
        if (m == move::north || m == move::south) {
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
    grid new_grid = g;

    for (const auto [row, col] : boxes) {
        new_grid[row][col] = tile_type::empty;
    }
    for (const auto [row, col] : boxes) {
        new_grid[row + dir.first][col + dir.second] = g[row][col];
    }

    g = std::move(new_grid);

    return true;
}

void simulate(grid &grid, const std::vector<move> &moves) {
    auto [robot_row, robot_col] = find_robot(grid);

    for (const auto m : moves) {
        const auto direction = move_to_direction(m);

        std::size_t robot_dest_row = robot_row + direction.first;
        std::size_t robot_dest_col = robot_col + direction.second;

        const auto dest_tile = grid[robot_dest_row][robot_dest_col];
        if (dest_tile == tile_type::wall) {
            // Facing wall: do nothing
        } else if (dest_tile == tile_type::empty) {
            // Empty space: move one step
            grid[robot_row][robot_col] = tile_type::empty;
            grid[robot_dest_row][robot_dest_col] = tile_type::robot;
            robot_row = robot_dest_row;
            robot_col = robot_dest_col;
        } else if (dest_tile == tile_type::box || dest_tile == tile_type::big_box_left ||
                   dest_tile == tile_type::big_box_right) {
            if (move_box(robot_dest_row, robot_dest_col, m, grid)) {
                grid[robot_row][robot_col] = tile_type::empty;
                grid[robot_dest_row][robot_dest_col] = tile_type::robot;
                robot_row = robot_dest_row;
                robot_col = robot_dest_col;
            }
        }
    }
}

std::size_t sum_of_box_gps_coordinates(const grid &grid) {
    std::size_t total = 0;
    for (std::size_t row = 0; row < grid.size(); ++row) {
        for (std::size_t col = 0; col < grid[row].size(); ++col) {
            if (grid[row][col] == tile_type::box || grid[row][col] == tile_type::big_box_left) {
                total += 100 * row + col;
            }
        }
    }
    return total;
}

void print_grid(const grid &grid) {
    for (const auto &row : grid) {
        for (const auto tile : row) {
            std::print("{}", static_cast<char>(tile));
        }
        std::print("\n");
    }
}

int main() {
    const auto grid = read_grid(std::cin);
    const auto moves = read_moves(std::cin);

    auto grid_copy = grid;
    simulate(grid_copy, moves);

    std::println("Part 1: {}", sum_of_box_gps_coordinates(grid_copy));

    auto wide_grid = convert_to_wide_grid(grid);
    simulate(wide_grid, moves);

    std::println("Part 2: {}", sum_of_box_gps_coordinates(wide_grid));
    return 0;
}
