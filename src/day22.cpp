#include <algorithm>
#include <boost/functional/hash.hpp>
#include <iostream>
#include <print>
#include <vector>

std::vector<std::size_t> read_input(std::istream &is) {
    std::vector<std::size_t> secret_numbers;
    std::string line;
    while (std::getline(is, line)) {
        secret_numbers.push_back(std::stoul(line));
    }
    return secret_numbers;
}

constexpr std::size_t generate_next(std::size_t secret_number) {
    constexpr std::size_t prune_mask = (1 << 24) - 1;

    // Step 1
    secret_number ^= (secret_number << 6uz);
    secret_number &= prune_mask;
    // Step 2
    secret_number ^= (secret_number >> 5uz);
    secret_number &= prune_mask;
    // Step 3
    secret_number ^= (secret_number << 11uz);
    secret_number &= prune_mask;

    return secret_number;
}

constexpr std::size_t generate_nth(std::size_t secret_number, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        secret_number = generate_next(secret_number);
    }
    return secret_number;
}

using diff_sequence = std::array<int8_t, 4>;

constexpr std::size_t map_diff_sequence_to_index(const diff_sequence &seq) {
    std::size_t total = 0;
    std::size_t base = 1;
    for (std::size_t i = 0; i < 4; ++i) {
        total += (seq[i] + 9uz) * base;
        base *= 19uz;
    }
    return total;
}

template <std::size_t N>
void get_earnings(std::size_t secret_number, std::size_t n, std::array<std::size_t, N> &price) {
    std::array<bool, N> sold{};

    diff_sequence current_seq = {0, 0, 0, 0};
    for (std::size_t i = 0; i < n; ++i) {
        const auto current_price = secret_number % 10;
        const auto next_number = generate_next(secret_number);
        const auto next_price = next_number % 10;
        const auto difference = static_cast<int8_t>(next_price) - current_price;

        current_seq[0] = current_seq[1];
        current_seq[1] = current_seq[2];
        current_seq[2] = current_seq[3];
        current_seq[3] = difference;

        // Sell only on first match
        if (const auto idx = map_diff_sequence_to_index(current_seq); i >= 3 && !sold[idx]) {
            price[idx] += next_number % 10;
            sold[idx] = true;
        }
        secret_number = next_number;
    }
}

int main() {
    const auto secret_numbers = read_input(std::cin);

    std::size_t total = 0;
    for (const auto num : secret_numbers) {
        total += generate_nth(num, 2000);
    }
    std::println("Part 1: {}", total);

    // Part 2 (brute force)
    constexpr std::size_t array_size = map_diff_sequence_to_index({9uz, 9uz, 9uz, 9uz}) + 1uz;
    std::array<std::size_t, array_size> price_total{};
    for (const auto num : secret_numbers) {
        get_earnings(num, 2000, price_total);
    }
    std::println("Part 2: {}", std::ranges::max(price_total));

    return 0;
}
