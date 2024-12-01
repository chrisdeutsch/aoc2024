#include "parsing.h"
#include <algorithm>
#include <iostream>

int main(int argc, char *argv[]) {
    const auto input = parse_input<int, int>(std::cin);

    std::vector<int> first, second;
    for (auto [a, b] : input) {
        first.push_back(a);
        second.push_back(b);
    }
    std::sort(first.begin(), first.end());
    std::sort(second.begin(), second.end());

    int total_distance = 0;
    for (auto it1 = first.cbegin(), it2 = second.cbegin();
         it1 != first.cend() && it2 != second.cend(); it1++, it2++) {
        const auto dist = std::abs(*it1 - *it2);
        total_distance += dist;
    }

    std::clog << "Part 1: " << total_distance << "\n";

    return 0;
}
