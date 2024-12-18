#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <map>
#include <variant>
#include <vector>

using IntType = long long;

std::vector<IntType> read_input(std::istream &istream) {
    std::vector<IntType> result;

    auto begin = std::istream_iterator<IntType>(istream);
    auto end = std::istream_iterator<IntType>();
    std::copy(begin, end, std::back_inserter(result));

    return result;
}

std::size_t get_num_digits(IntType num) {
    std::size_t digits = 0;
    while (num >= 1) {
        num /= 10;
        ++digits;
    }
    return digits;
}

IntType intpow(IntType base, std::size_t exp) {
    IntType result = 1;
    for (; exp > 0; --exp) {
        result *= base;
    }
    return result;
}

std::variant<IntType, std::pair<IntType, IntType>> blink(IntType stone) {
    if (stone == 0) {
        return 1;
    }

    const auto num_digits = get_num_digits(stone);
    if (num_digits % 2 == 0) {
        const auto power = intpow(10, num_digits / 2);
        return std::make_pair(stone / power, stone % power);
    }

    return 2024 * stone;
}

std::vector<IntType> blink(std::vector<IntType>::const_iterator begin,
                           std::vector<IntType>::const_iterator end) {
    std::vector<IntType> stones;
    stones.reserve(2 * std::distance(begin, end));

    for (auto it = begin; it != end; ++it) {
        auto result = blink(*it);
        if (const auto ptr = std::get_if<IntType>(&result)) {
            stones.push_back(*ptr);
        } else if (const auto ptr = std::get_if<std::pair<IntType, IntType>>(&result)) {
            stones.push_back(ptr->first);
            stones.push_back(ptr->second);
        }
    }
    return stones;
}

std::size_t count_stones(IntType stone, std::size_t depth, std::size_t depth_limit,
                         std::map<std::pair<IntType, std::size_t>, std::size_t> &mem) {
    // Memoized?
    const auto key = std::make_pair(stone, depth);
    if (const auto it = mem.find(key); it != mem.cend()) {
        return it->second;
    }

    // Calculate result
    std::size_t num_stones;
    if (depth >= depth_limit) {
        num_stones = 1;
    } else {
        const auto result = blink(stone);
        if (const auto result_ptr = std::get_if<IntType>(&result)) {
            num_stones = count_stones(*result_ptr, depth + 1, depth_limit, mem);
        }
        if (const auto result_ptr = std::get_if<std::pair<IntType, IntType>>(&result)) {
            num_stones = count_stones(result_ptr->first, depth + 1, depth_limit, mem) +
                         count_stones(result_ptr->second, depth + 1, depth_limit, mem);
        }
    }

    // Memoize!
    mem[key] = num_stones;
    return num_stones;
}

int main() {
    const auto input = read_input(std::cin);

    std::vector<IntType> stones(input.cbegin(), input.cend());
    for (std::size_t num_blink = 0; num_blink < 25; ++num_blink) {
        stones = blink(stones.cbegin(), stones.cend());
    }
    std::cout << "Part 1: " << stones.size() << "\n";

    std::size_t total = 0;
    std::map<std::pair<IntType, std::size_t>, std::size_t> mem;
    for (const auto stone : input) {
        total += count_stones(stone, 0, 75, mem);
    }
    std::cout << "Part 2: " << total << "\n";

    return 0;
}
