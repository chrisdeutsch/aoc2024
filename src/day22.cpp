#include <boost/functional/hash.hpp>
#include <iostream>
#include <print>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
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

template <> struct std::hash<diff_sequence> {
    std::size_t operator()(const diff_sequence &seq) const {
        std::size_t seed = 0;
        boost::hash_combine(seed, boost::hash_value(seq[0]));
        boost::hash_combine(seed, boost::hash_value(seq[1]));
        boost::hash_combine(seed, boost::hash_value(seq[2]));
        boost::hash_combine(seed, boost::hash_value(seq[3]));
        return seed;
    }
};

std::unordered_map<diff_sequence, uint8_t> get_thing(std::size_t secret_number, std::size_t n) {
    std::unordered_map<diff_sequence, uint8_t> m;

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

        // Sell on first match
        if (i >= 3 && !m.contains(current_seq)) {
            m[current_seq] = next_number % 10;
        }
        secret_number = next_number;
    }

    return m;
}

int main() {
    const auto secret_numbers = read_input(std::cin);

    std::size_t total = 0;
    for (const auto num : secret_numbers) {
        total += generate_nth(num, 2000);
    }
    std::println("Part 1: {}", total);

    // Part 2 (brute force)
    std::vector<std::unordered_map<diff_sequence, uint8_t>> earnings_map;
    earnings_map.reserve(secret_numbers.size());

    for (const auto num : secret_numbers) {
        earnings_map.emplace_back(get_thing(num, 2000));
    }

    std::unordered_set<diff_sequence> sequences_seen;
    for (const auto &sequence_to_price : earnings_map) {
        std::transform(sequence_to_price.cbegin(), sequence_to_price.cend(),
                       std::inserter(sequences_seen, std::end(sequences_seen)),
                       [](const auto &elem) { return elem.first; });
    }

    std::size_t max = 0;
    for (const auto &sequence : sequences_seen) {
        std::size_t total = 0;
        for (const auto &elem : earnings_map) {
            if (auto it = elem.find(sequence); it != elem.cend()) {
                total += it->second;
            }
        }
        max = std::max(max, total);
    }
    std::println("Part 2: {}", max);

    return 0;
}
