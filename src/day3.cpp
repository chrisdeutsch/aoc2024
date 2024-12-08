#include <iostream>
#include <iterator>
#include <regex>
#include <string>

std::string read_input(std::istream &istream) {
    std::istream_iterator<char> it(istream);
    return std::string(it, {});
}

int sum_of_muls(const std::string &s) {
    std::regex mul_regex("mul\\((\\d{1,3}),(\\d{1,3})\\)");

    auto regex_begin = std::sregex_iterator(s.cbegin(), s.cend(), mul_regex);
    auto regex_end = std::sregex_iterator();

    int total = 0;
    for (auto it = regex_begin; it != regex_end; ++it) {
        auto match = *it;

        auto fac1 = std::stoi(match[1]);
        auto fac2 = std::stoi(match[2]);
        total += fac1 * fac2;
    }
    return total;
}

int sum_of_muls_do_dont(const std::string &s) {
    std::regex mul_do_dont_regex("do\\(\\)|don't\\(\\)|mul\\((\\d{1,3}),(\\d{1,3})\\)");

    auto regex_begin = std::sregex_iterator(s.cbegin(), s.cend(), mul_do_dont_regex);
    auto regex_end = std::sregex_iterator();

    int total = 0;
    bool enabled = true;
    for (auto it = regex_begin; it != regex_end; ++it) {
        auto match = *it;
        if (match.str() == "do()") {
            enabled = true;
        } else if (match.str() == "don't()") {
            enabled = false;
        } else if (enabled) {
            auto fac1 = std::stoi(match[1]);
            auto fac2 = std::stoi(match[2]);
            total += fac1 * fac2;
        }
    }
    return total;
}

int main() {
    const auto input = read_input(std::cin);
    const auto part1 = sum_of_muls(input);
    const auto part2 = sum_of_muls_do_dont(input);

    std::clog << "Part 1: " << part1 << "\n";
    std::clog << "Part 2: " << part2 << "\n";
    return 0;
}
