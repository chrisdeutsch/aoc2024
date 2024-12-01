#include "parsing.h"
#include <algorithm>
#include <iostream>
#include <map>

int main(int argc, char *argv[]) {
    const auto input = parse_input<int, int>(std::cin);

    std::vector<int> first, second;
    for (auto [a, b] : input) {
        first.push_back(a);
        second.push_back(b);
    }
    std::sort(first.begin(), first.end());
    std::sort(second.begin(), second.end());

    std::size_t total_distance = 0;
    for (auto it1 = first.cbegin(), it2 = second.cbegin();
         it1 != first.cend() && it2 != second.cend(); it1++, it2++) {
        const std::size_t dist = std::abs(*it1 - *it2);
        total_distance += dist;
    }

    std::clog << "Part 1: " << total_distance << "\n";

    std::map<int, std::size_t> counts;
    for (auto item : second) {
        counts[item] += 1;
    }

    for (auto [k, v] : counts) {
        std::clog << k << " " << v << "\n";
    }

    std::size_t similarity = 0;
    for (auto item : first) {
        const auto it = counts.find(item);
        if (it != counts.cend()) {
            similarity += item * it->second;
        }
    }

    std::clog << "Part 2: " << similarity << "\n";

    return 0;
}
