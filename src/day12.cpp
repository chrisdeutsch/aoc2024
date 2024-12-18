#include <iostream>
#include <print>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using garden_plot = std::vector<std::vector<char>>;

garden_plot read_input(std::istream &istream) {
    garden_plot gp;
    std::string line;
    while (std::getline(istream, line)) {
        gp.emplace_back(line.cbegin(), line.cend());
    }
    return gp;
}

struct tile {
    char value;
    std::size_t row;
    std::size_t col;

    std::size_t num_neighbors = 0;
    bool operator<(const tile &other) const {
        return std::tie(this->row, this->col) < std::tie(other.row, other.col);
    }
};

std::vector<tile> get_region(std::size_t start_row, std::size_t start_col,
                             const garden_plot &plot) {
    const auto num_rows = plot.size();
    const auto num_cols = plot[0].size();
    const auto tile_value = plot[start_row][start_col];

    std::vector<tile> region;
    std::set<tile> visited;

    std::vector<tile> stack;
    stack.emplace_back(tile{.value = tile_value, .row = start_row, .col = start_col});

    while (!stack.empty()) {
        auto t = stack.back();
        stack.pop_back();
        if (visited.contains(t)) {
            continue;
        }

        // North
        if (t.row >= 1 && plot[t.row - 1][t.col] == tile_value) {
            stack.emplace_back(
                tile{.value = plot[t.row - 1][t.col], .row = t.row - 1, .col = t.col});
            ++t.num_neighbors;
        }
        // East
        if (t.col + 1 < num_cols && plot[t.row][t.col + 1] == tile_value) {
            stack.emplace_back(
                tile{.value = plot[t.row][t.col + 1], .row = t.row, .col = t.col + 1});
            ++t.num_neighbors;
        }
        // South
        if (t.row + 1 < num_rows && plot[t.row + 1][t.col] == tile_value) {
            stack.emplace_back(
                tile{.value = plot[t.row + 1][t.col], .row = t.row + 1, .col = t.col});
            ++t.num_neighbors;
        }
        // West
        if (t.col >= 1 && plot[t.row][t.col - 1] == tile_value) {
            stack.emplace_back(
                tile{.value = plot[t.row][t.col - 1], .row = t.row, .col = t.col - 1});
            ++t.num_neighbors;
        }

        region.push_back(t);
        visited.insert(t);
    }

    return region;
}

std::vector<std::vector<tile>> get_all_regions(const garden_plot &plot) {
    const auto num_rows = plot.size();
    const auto num_cols = plot[0].size();

    std::set<std::pair<std::size_t, std::size_t>> visited;
    std::vector<std::vector<tile>> regions;

    for (std::size_t row = 0; row < num_rows; ++row) {
        for (std::size_t col = 0; col < num_cols; ++col) {
            if (visited.contains({row, col})) {
                continue;
            }

            const auto region = get_region(row, col, plot);
            for (const auto &t : region) {
                visited.insert({t.row, t.col});
            }
            regions.emplace_back(std::move(region));
        }
    }

    return regions;
}

std::size_t get_perimeter(std::vector<tile> region) {
    std::size_t total = 0;
    for (const auto &t : region) {
        total += 4 - t.num_neighbors;
    }
    return total;
}

int main() {
    const auto plot = read_input(std::cin);
    const auto regions = get_all_regions(plot);

    std::size_t total_fence = 0;
    for (const auto &region : regions) {
        const auto area = region.size();
        const auto perimeter = get_perimeter(region);

        total_fence += area * perimeter;
    }

    std::println("Part 1: {}", total_fence);

    return 0;
}
