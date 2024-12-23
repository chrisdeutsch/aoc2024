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

bool is_pattern_possible(const std::string_view desired,
                         const std::unordered_set<std::string_view> &available_patterns,
                         const std::size_t max_pattern_length,
                         std::unordered_map<std::string_view, bool> &mem) {
    if (desired.size() == 0) {
        return true;
    }

    if (auto it = mem.find(desired); it != mem.cend()) {
        return it->second;
    }

    for (std::size_t pattern_length = 1;
         pattern_length <= std::min(desired.size(), max_pattern_length); ++pattern_length) {
        const auto desired_substr = desired.substr(0, pattern_length);
        if (available_patterns.contains(desired_substr) &&
            is_pattern_possible(desired.substr(pattern_length), available_patterns,
                                max_pattern_length, mem)) {
            mem[desired] = true;
            return true;
        }
    }
    mem[desired] = false;
    return false;
}

int main() {
    const auto [available_patterns, desired_patterns] = read_input(std::cin);

    const auto max_pattern_length = std::ranges::max(
        available_patterns | std::views::transform([](const auto &s) { return s.size(); }));
    std::unordered_set<std::string_view> available_patterns_set(available_patterns.cbegin(),
                                                                available_patterns.cend());

    std::unordered_map<std::string_view, bool> mem;
    std::size_t num_possible = 0;
    for (const auto &pattern : desired_patterns) {
        if (is_pattern_possible(pattern, available_patterns_set, max_pattern_length, mem)) {
            ++num_possible;
        }
    }
    std::println("Part 1: {}", num_possible);

    return 0;
}
