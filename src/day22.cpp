#include <iostream>
#include <map>
#include <print>
#include <ranges>
#include <set>
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

std::vector<uint8_t> get_prices(std::size_t secret_number, std::size_t n) {
    std::vector<uint8_t> prices = {static_cast<uint8_t>(secret_number % 10)};
    prices.reserve(n + 1);

    for (std::size_t i = 0; i < n; ++i) {
        secret_number = generate_next(secret_number);
        prices.push_back(secret_number % 10);
    }

    return prices;
}

std::vector<int8_t> get_differences(const std::span<const uint8_t> prices) {
    std::vector<int8_t> differences;
    for (const auto [a, b] : std::views::zip(prices, prices | std::views::drop(1))) {
        differences.push_back(static_cast<int8_t>(b) - a);
    }
    return differences;
}

int main() {
    const auto secret_numbers = read_input(std::cin);

    std::size_t total = 0;
    for (const auto num : secret_numbers) {
        total += generate_nth(num, 2000);
    }
    std::println("Part 1: {}", total);

    // Part 2
    using diff_sequence = std::array<int8_t, 4>;
    std::set<diff_sequence> diff_sequences;

    for (const auto num : secret_numbers) {
        std::map<diff_sequence, uint8_t> sequence_to_price;

        const auto prices = get_prices(num, 2000);
        const auto differences = get_differences(prices);

        for (auto it_start = differences.cbegin(), it_end = differences.cbegin() + 4;
             it_end != differences.cend(); ++it_start, ++it_end) {
            const diff_sequence key = {it_start[0], it_start[1], it_start[2], it_start[3]};
            if (auto it = sequence_to_price.find(key); it == sequence_to_price.cend()) {
                const auto dist = std::distance(differences.cbegin(), it_start + 3);
                sequence_to_price[key] = prices[dist + 1];
            }
        }
    }
    std::println("{}", diff_sequences.size());

    /*const auto prices = get_prices(123, 10);*/
    /*const auto differences = get_differences(prices);*/
    /*for (auto it_start = differences.cbegin(), it_end = differences.cbegin() + 4;*/
    /*     it_end != differences.cend(); ++it_start, ++it_end) {*/
    /*    diff_sequences.emplace(diff_sequence{it_start[0], it_start[1], it_start[2],
     * it_start[3]});*/
    /*}*/
    /*std::println("{}", diff_sequences.size());*/

    return 0;
}
