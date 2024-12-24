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

int main() {
    const auto secret_numbers = read_input(std::cin);

    std::size_t total = 0;
    for (const auto num : secret_numbers) {
        total += generate_nth(num, 2000);
    }
    std::println("Part 1: {}", total);

    return 0;
}
