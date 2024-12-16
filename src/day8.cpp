#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <vector>

struct antenna {
    int row;
    int col;
    char frequency;
};

using antinode = antenna;

std::tuple<std::vector<antenna>, int, int> read_input(std::istream &istream) {
    std::vector<antenna> antennas;
    int row = 0;
    int col = 0;
    std::string line;
    while (istream && std::getline(istream, line)) {
        col = 0;
        for (const auto c : line) {
            if (c != '.') {
                antennas.push_back({
                    .row = row,
                    .col = col,
                    .frequency = c,
                });
            }
            col++;
        }
        row++;
    }
    return std::make_tuple(antennas, row, col);
}

std::map<char, std::vector<antenna>>
group_antenna_by_frequency(const std::vector<antenna> &antennas) {
    std::map<char, std::vector<antenna>> antennas_by_frequency;
    for (const auto &antenna : antennas) {
        antennas_by_frequency[antenna.frequency].push_back(antenna);
    }
    return antennas_by_frequency;
}

antinode get_antinode(const antenna &a, const antenna &b) {
    if (a.frequency != b.frequency) {
        throw std::runtime_error("called get_antinodes on antennas with different frequencies");
    }

    const auto delta_row = b.row - a.row;
    const auto delta_col = b.col - a.col;

    return {
        .row = b.row + delta_row,
        .col = b.col + delta_col,
        .frequency = b.frequency,
    };
}

std::vector<antinode> get_antinodes_pt2(const antenna &a, const antenna &b,
                                        std::function<bool(int, int)> f_valid_loc) {
    if (a.frequency != b.frequency) {
        throw std::runtime_error("called get_antinodes on antennas with different frequencies");
    }

    const auto delta_row = b.row - a.row;
    const auto delta_col = b.col - a.col;

    std::vector<antinode> antinodes;

    int row = b.row;
    int col = b.col;
    while (f_valid_loc(row, col)) {
        antinodes.push_back({
            .row = row,
            .col = col,
            .frequency = b.frequency,
        });

        row += delta_row;
        col += delta_col;
    }

    return antinodes;
}

int main() {
    const auto [antennas, num_rows, num_cols] = read_input(std::cin);
    const auto antennas_by_frequency = group_antenna_by_frequency(antennas);

    auto is_valid_location = [num_rows, num_cols](int row, int col) {
        return 0 <= row && row < num_rows && 0 <= col && col < num_cols;
    };

    std::set<std::pair<int, int>> antinode_locations_pt1;
    std::set<std::pair<int, int>> antinode_locations_pt2;
    for (const auto &[frequency, antennas] : antennas_by_frequency) {
        // All antenna pairs
        for (const auto &a : antennas) {
            for (const auto &b : antennas) {
                if (&a == &b) {
                    continue;
                }

                // Part 1
                const auto antinode = get_antinode(a, b);
                if (is_valid_location(antinode.row, antinode.col)) {
                    antinode_locations_pt1.emplace(antinode.row, antinode.col);
                }

                // Part 2
                const auto antinodes = get_antinodes_pt2(a, b, is_valid_location);
                for (const auto &an : antinodes) {
                    antinode_locations_pt2.emplace(an.row, an.col);
                }
            }
        }
    }

    std::clog << "Part 1: " << antinode_locations_pt1.size() << "\n";
    std::clog << "Part 2: " << antinode_locations_pt2.size() << "\n";

    return 0;
}
