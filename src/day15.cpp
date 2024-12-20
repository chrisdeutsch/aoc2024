#include <algorithm>
#include <iostream>
#include <print>
#include <string>
#include <vector>

enum class tile {
    wall = '#',
    empty = '.',
    box = 'O',
    robot = '@',
};

enum class move {
    north = '^',
    east = '>',
    south = 'v',
    west = '<',
};

using grid = std::vector<std::vector<tile>>;

grid read_grid(std::istream &istream) {
    grid g;

    std::string line;
    while (std::getline(istream, line) && line != "") {
        std::vector<tile> row;
        row.reserve(line.size());
        std::transform(line.cbegin(), line.cend(), std::back_inserter(row),
                       [](char ch) { return static_cast<tile>(ch); });
        g.emplace_back(std::move(row));
    }
    return g;
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
            if (grid[row][col] == tile::robot) {
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

void simulate(grid &grid, const std::vector<move> &moves) {
    auto [robot_row, robot_col] = find_robot(grid);

    for (const auto m : moves) {
        const auto direction = move_to_direction(m);

        std::size_t robot_dest_row = robot_row + direction.first;
        std::size_t robot_dest_col = robot_col + direction.second;

        const auto dest_tile = grid[robot_dest_row][robot_dest_col];
        if (dest_tile == tile::wall) {
            // Facing wall: do nothing
        } else if (dest_tile == tile::empty) {
            // Empty space: move one step
            grid[robot_row][robot_col] = tile::empty;
            grid[robot_dest_row][robot_dest_col] = tile::robot;
            robot_row = robot_dest_row;
            robot_col = robot_dest_col;
        } else if (dest_tile == tile::box) {
            // Facing box: search for the next empty or wall tile in move direction
            auto search_row = robot_dest_row;
            auto search_col = robot_dest_col;
            while (grid[search_row][search_col] == tile::box) {
                search_row += direction.first;
                search_col += direction.second;
            }
            // Move all boxes & robot one step in direction
            if (grid[search_row][search_col] == tile::empty) {
                grid[search_row][search_col] = tile::box;
                grid[robot_row][robot_col] = tile::empty;
                grid[robot_dest_row][robot_dest_col] = tile::robot;
                robot_row = robot_dest_row;
                robot_col = robot_dest_col;
            }
            // Fallthrough: boxes connect to wall -> do nothing
        }
    }
}

std::size_t sum_of_box_gps_coordinates(const grid &grid) {
    std::size_t total = 0;
    for (std::size_t row = 0; row < grid.size(); ++row) {
        for (std::size_t col = 0; col < grid[row].size(); ++col) {
            if (grid[row][col] == tile::box) {
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

    return 0;
}
