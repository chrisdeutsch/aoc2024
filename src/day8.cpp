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

int main() {
    const auto [antennas, num_rows, num_cols] = read_input(std::cin);
    const auto antennas_by_frequency = group_antenna_by_frequency(antennas);

    auto is_valid_location = [num_rows, num_cols](int row, int col) {
        return 0 <= row && row < num_rows && 0 <= col && col < num_cols;
    };

    std::vector<antinode> antinodes;
    for (const auto &[frequency, antennas] : antennas_by_frequency) {
        // All antenna pairs
        for (const auto &a : antennas) {
            for (const auto &b : antennas) {
                if (&a == &b) {
                    continue;
                }
                const auto antinode = get_antinode(a, b);
                if (is_valid_location(antinode.row, antinode.col)) {
                    antinodes.push_back(antinode);
                }
            }
        }
    }

    std::set<std::pair<int, int>> unique_antinode_locations;
    for (const auto &antinode : antinodes) {
        unique_antinode_locations.insert(std::make_pair(antinode.row, antinode.col));
    }

    std::clog << "Part 1: " << unique_antinode_locations.size() << "\n";

    return 0;
}
