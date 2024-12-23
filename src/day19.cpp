#include <absl/strings/str_split.h>
#include <algorithm>
#include <iostream>
#include <print>
#include <ranges>
#include <unordered_map>
#include <unordered_set>

std::pair<std::vector<std::string>, std::vector<std::string>> read_input(std::istream &is) {
    std::string line;

    // Available patterns
    std::vector<std::string> available_patterns;
    if (std::getline(is, line)) {
        available_patterns = absl::StrSplit(line, ", ");
    } else {
        throw std::runtime_error("Could not parse available patterns");
    }

    // Desired patterns
    std::vector<std::string> desired_patterns;
    while (std::getline(is, line)) {
        if (line.empty()) {
            continue;
        }
        desired_patterns.push_back(line);
    }

    return {std::move(available_patterns), std::move(desired_patterns)};
}

std::size_t get_number_of_pattern_combinations(
    const std::string_view desired, const std::unordered_set<std::string_view> &available_patterns,
    const std::size_t max_pattern_length, std::unordered_map<std::string_view, std::size_t> &mem) {
    if (desired.size() == 0) {
        return 1;
    }

    if (auto it = mem.find(desired); it != mem.cend()) {
        return it->second;
    }

    std::size_t num_patterns = 0;
    for (std::size_t pattern_length = 1;
         pattern_length <= std::min(desired.size(), max_pattern_length); ++pattern_length) {
        const auto desired_substr = desired.substr(0, pattern_length);
        if (available_patterns.contains(desired_substr)) {
            num_patterns += get_number_of_pattern_combinations(
                desired.substr(pattern_length), available_patterns, max_pattern_length, mem);
        }
    }
    mem[desired] = num_patterns;
    return num_patterns;
}

int main() {
    const auto [available_patterns, desired_patterns] = read_input(std::cin);

    const auto max_pattern_length = std::ranges::max(
        available_patterns | std::views::transform([](const auto &s) { return s.size(); }));
    std::unordered_set<std::string_view> available_patterns_set(available_patterns.cbegin(),
                                                                available_patterns.cend());

    std::unordered_map<std::string_view, std::size_t> mem;

    std::size_t num_pattern_possible = 0;
    std::size_t sum_pattern_combinations = 0;
    for (const auto &pattern : desired_patterns) {
        const auto num_comb = get_number_of_pattern_combinations(pattern, available_patterns_set,
                                                                 max_pattern_length, mem);
        sum_pattern_combinations += num_comb;
        if (num_comb > 0) {
            ++num_pattern_possible;
        }
    }
    std::println("Part 1: {}", num_pattern_possible);
    std::println("Part 1: {}", sum_pattern_combinations);

    return 0;
}
