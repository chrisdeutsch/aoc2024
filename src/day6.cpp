#include <algorithm>
#include <iostream>
#include <optional>
#include <set>
#include <string>
#include <vector>

struct guard_pos {
    int row;
    int col;
    int dir_row;
    int dir_col;

    bool operator<(const guard_pos &other) const {
        return std::tie(this->row, this->col, this->dir_row, this->dir_col) <
               std::tie(other.row, other.col, other.dir_row, other.dir_col);
    }
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

// std::vector<std::vector<bool>>
std::optional<std::size_t> walk(guard_pos guard,
                                const std::vector<std::vector<bool>> &obstructions) {
    const auto n_rows = obstructions.size();
    const auto n_cols = obstructions.front().size();

    std::vector<std::vector<bool>> visited;
    for (std::size_t i; i < n_rows; i++) {
        visited.emplace_back(n_cols);
    }
    visited[guard.row][guard.col] = true;

    std::set<guard_pos> prev_pos;

    auto is_valid_pos = [n_rows, n_cols](int row, int col) {
        return 0 <= row && row < n_rows && 0 <= col && col < n_cols;
    };

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
            // Walked off map
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

        // Loop detection
        if (prev_pos.find(guard) != prev_pos.cend()) {
            return {};
        } else {
            prev_pos.insert(guard);
        }
    }

    std::size_t num_visited = 0;
    for (const auto &row : visited) {
        for (const auto col : row) {
            if (col) {
                num_visited++;
            }
        }
    }

    return num_visited;
}

int main() {
    auto [obstructions, guard] = read_input(std::cin);
    const auto num_visited = *walk(guard, obstructions);
    std::clog << "Part 1: " << num_visited << "\n";

    std::size_t num_loops = 0;
    const auto num_rows = obstructions.size();
    const auto num_cols = obstructions.front().size();
    for (std::size_t row = 0; row < num_rows; row++) {
        for (std::size_t col = 0; col < num_cols; col++) {
            if (row == guard.row && col == guard.col) {
                continue;
            }
            if (obstructions[row][col]) {
                // Nothing to do
                continue;
            }

            obstructions[row][col] = true;
            if (!walk(guard, obstructions)) {
                num_loops += 1;
            }
            obstructions[row][col] = false;
        }
    }

    std::clog << "Part 2: " << num_loops << "\n";

    return 0;
}
