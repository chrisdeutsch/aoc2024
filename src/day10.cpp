#include <iostream>
#include <vector>
#include <algorithm>
#include <optional>
#include <set>

using topographic_map = std::vector<std::vector<std::size_t>>;

topographic_map read_input(std::istream &istream) {
    topographic_map map;

    std::string line;
    while (std::getline(istream, line)) {
        std::vector<std::size_t> heights;
        heights.reserve(line.size());

        std::transform(line.cbegin(), line.cend(), std::back_inserter(heights), [](char c) { return static_cast<std::size_t>(c - '0'); });
        map.emplace_back(std::move(heights));
    }
    return map;
}

void print_map(const topographic_map &map) {
    for (const auto &row : map) {
        for (const auto col : row) {
            std::clog << col;
        }
        std::clog << "\n";
    }
}

std::set<std::pair<std::size_t, std::size_t>> step(std::size_t row, std::size_t col, const topographic_map &map) {
    const auto height = map[row][col];
    if (height == 9) {
        return {std::make_pair(row, col)};
    }

    std::set<std::pair<std::size_t, std::size_t>> peaks;

    // North
    if (row > 0 && map[row - 1][col] == height + 1) {
        peaks.merge(step(row - 1, col, map));
    }
    // East
    if (col < map[row].size() - 1 && map[row][col + 1] == height + 1) {
        peaks.merge(step(row, col + 1, map));
    }
    // South
    if (row < map.size() - 1 && map[row + 1][col] == height + 1) {
        peaks.merge(step(row + 1, col, map));
    }
    // West
    if (col > 0 && map[row][col - 1] == height + 1) {
        peaks.merge(step(row, col - 1, map));
    }

    return peaks;
}


int main() {
    const auto map = read_input(std::cin);

    std::size_t total = 0;
    for (std::size_t row = 0; row < map.size(); ++row) {
        for (std::size_t col = 0; col < map[row].size(); ++col) {
            const auto height = map[row][col];
            if (height != 0) {
                continue;
            }
            const auto trailhead_score = step(row, col, map).size();
            total += trailhead_score;
        }
    }

    std::cout << "Part 1: " << total << "\n";

    return 0;
}
