#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

struct guard_pos {
    int row;
    int col;
    int dir_row;
    int dir_col;
};

std::pair<std::vector<std::vector<bool>>, guard_pos> read_input(std::istream &istream) {
    std::vector<std::vector<bool>> obstructions;
    guard_pos guard;

    std::size_t n_row = 0;
    std::string line;
    while (std::getline(istream, line)) {
        // Parse obstructions
        std::vector<bool> row;
        std::transform(line.cbegin(), line.cend(), std::back_inserter(row),
                       [](const char c) { return c == '#'; });
        obstructions.emplace_back(std::move(row));

        // Find guard
        auto guard_col = line.find('^');
        if (guard_col != std::string::npos) {
            guard = {.row = static_cast<int>(n_row),
                     .col = static_cast<int>(guard_col),
                     .dir_row = -1,
                     .dir_col = 0};
        }
        n_row++;
    }
    return std::make_pair(obstructions, guard);
}

std::vector<std::vector<bool>> walk(guard_pos guard,
                                    const std::vector<std::vector<bool>> &obstructions) {
    const auto n_rows = obstructions.size();
    const auto n_cols = obstructions.front().size();

    std::vector<std::vector<bool>> visited;
    for (std::size_t i; i < n_rows; i++) {
        visited.emplace_back(n_cols);
    }

    auto is_valid_pos = [n_rows, n_cols](int row, int col) {
        return 0 <= row && row < n_rows && 0 <= col && col < n_cols;
    };

    visited[guard.row][guard.col] = true;

    while (true) {
        guard_pos next = {
            .row = guard.row + guard.dir_row,
            .col = guard.col + guard.dir_col,
            .dir_row = guard.dir_row,
            .dir_col = guard.dir_col,
        };

        const auto is_valid = is_valid_pos(next.row, next.col);
        const auto is_obstruction = obstructions[next.row][next.col];

        if (!is_valid) {
            break;
        }
        if (is_obstruction) {
            // Turn
            const auto new_dir_row = guard.dir_col;
            const auto new_dir_col = -guard.dir_row;
            guard.dir_row = new_dir_row;
            guard.dir_col = new_dir_col;
            continue;
        }

        guard = next;
        visited[guard.row][guard.col] = true;
    }

    return visited;
}

int main() {
    const auto [obstructions, guard] = read_input(std::cin);
    const auto visited = walk(guard, obstructions);

    std::size_t num_visited = 0;
    for (const auto &row : visited) {
        for (const auto col : row) {
            if (col) {
                num_visited++;
            }
        }
    }

    std::clog << "Part 1: " << num_visited << "\n";
    return 0;
}
