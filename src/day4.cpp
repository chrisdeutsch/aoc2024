#include <iostream>
#include <vector>

std::vector<std::string> read_input(std::istream &istream) {
    std::vector<std::string> result;
    std::string line;
    while (std::getline(istream, line)) {
        result.push_back(line);
    }
    return result;
}

bool is_xmas(char a, char b, char c, char d) {
    return (a == 'X' && b == 'M' && c == 'A' && d == 'S') ||
           (a == 'S' && b == 'A' && c == 'M' && d == 'X');
}

std::size_t count_xmas(const std::vector<std::string> input) {
    const auto num_rows = input.size();
    const auto num_cols = input.front().size();

    std::size_t num_xmas = 0;
    for (std::size_t row = 0; row < num_rows; row++) {
        for (std::size_t col = 0; col < num_cols; col++) {
            if (col + 3 < num_cols) {
                num_xmas += is_xmas(input[row][col], input[row][col + 1], input[row][col + 2],
                                    input[row][col + 3]);
            }
            if (row + 3 < num_rows) {
                num_xmas += is_xmas(input[row][col], input[row + 1][col], input[row + 2][col],
                                    input[row + 3][col]);
            }
            if (col + 3 < num_cols && row + 3 < num_rows) {
                num_xmas += is_xmas(input[row][col], input[row + 1][col + 1],
                                    input[row + 2][col + 2], input[row + 3][col + 3]);
            }
            if (col >= 3 && row + 3 < num_rows) {
                num_xmas += is_xmas(input[row][col], input[row + 1][col - 1],
                                    input[row + 2][col - 2], input[row + 3][col - 3]);
            }
        }
    }
    return num_xmas;
}

int main() {
    const auto input = read_input(std::cin);

    std::clog << "Part 1: " << count_xmas(input) << "\n";

    return 0;
}
