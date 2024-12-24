#include <absl/strings/str_split.h>
#include <iostream>
#include <print>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct computer {
    std::string name;
    std::set<std::string> connections;
    bool operator==(const computer &other) const { return name == other.name; }
};

template <> struct std::hash<computer> {
    std::size_t operator()(const computer &c) const { return std::hash<std::string>{}(c.name); }
};

struct connection {
    std::string first;
    std::string second;
};

std::vector<connection> read_input(std::istream &is) {
    std::vector<connection> connections;
    std::string line;
    while (std::getline(is, line)) {
        const std::pair<std::string, std::string> conn = absl::StrSplit(line, "-");
        connections.emplace_back(conn.first, conn.second);
    }
    return connections;
}

std::tuple<std::string, std::string, std::string> sort_triple(std::string a, std::string b,
                                                              std::string c) {
    if (a > b) {
        std::swap(a, b);
    }
    if (b > c) {
        std::swap(b, c);
    }
    if (a > b) {
        std::swap(a, b);
    }
    return {std::move(a), std::move(b), std::move(c)};
}

int main() {
    const auto connections = read_input(std::cin);

    std::unordered_map<std::string, computer> computers;

    // Create computers
    for (const auto &conn : connections) {
        computers[conn.first] = computer{.name = conn.first, .connections = {}};
        computers[conn.second] = computer{.name = conn.second, .connections = {}};
    }
    // Connect computers
    for (const auto &conn : connections) {
        computers[conn.first].connections.insert(conn.second);
        computers[conn.second].connections.insert(conn.first);
    }
    // Filter
    std::vector<computer> computers_with_t;
    for (const auto &[name, c] : computers) {
        if (name[0] != 't') {
            continue;
        }
        computers_with_t.push_back(c);
    }
    // Find triples
    std::set<std::tuple<std::string, std::string, std::string>> triples;
    while (!computers_with_t.empty()) {
        const auto computer_0 = computers_with_t.back();
        computers_with_t.pop_back();
        const auto &cname_0 = computer_0.name;

        std::set<std::string> connects = computer_0.connections;
        for (const auto &cname_1 : connects) {
            const auto &computer_1 = computers.at(cname_1);
            for (const auto &cname_2 : computer_1.connections) {
                if (connects.contains(cname_2)) {
                    triples.insert(sort_triple(cname_0, cname_1, cname_2));
                }
            }
        }
    }

    std::println("Part 1: {}", triples.size());

    return 0;
}
