#include <absl/strings/str_split.h>
#include <iostream>
#include <optional>
#include <string>

struct rule {
    const std::string lhs;
    const std::string rhs;
};

using pages = std::vector<std::string>;

std::pair<std::vector<rule>, std::vector<pages>> read_input(std::istream &istream) {
    std::vector<rule> rules;
    std::vector<pages> manuals;

    std::string line;
    while (std::getline(istream, line)) {
        if (line == "") {
            continue;
        }

        // Rules
        const std::pair<std::string, std::string> rule = absl::StrSplit(line, "|");
        if (rule.second.size() > 0) {
            rules.push_back({.lhs = rule.first, .rhs = rule.second});
            continue;
        }

        // Manuals
        const pages p = absl::StrSplit(line, ",");
        if (p.size() > 0) {
            manuals.emplace_back(std::move(p));
        }
    }

    return std::make_pair(rules, manuals);
}

std::optional<bool> less_than(const std::string &lhs, const std::string &rhs,
                              const std::map<std::pair<std::string, std::string>, bool> &rules) {
    const auto it = rules.find(std::make_pair(lhs, rhs));
    if (it != rules.cend()) {
        return it->second;
    }
    return {};
}

int main() {
    const auto [rules, manuals] = read_input(std::cin);

    std::map<std::pair<std::string, std::string>, bool> rulemap;
    for (const auto &rule : rules) {
        rulemap[std::make_pair(rule.lhs, rule.rhs)] = true;
        rulemap[std::make_pair(rule.rhs, rule.lhs)] = false;
    }

    auto fn_comp = [&rulemap](const auto &lhs, const auto &rhs) {
        auto lt = less_than(lhs, rhs, rulemap);
        if (!lt) {
            throw std::runtime_error("Encountered unknown comparison");
        }
        return *lt;
    };

    int total_pt1 = 0;
    int total_pt2 = 0;
    for (const auto &manual : manuals) {
        auto sorted_copy = manual;
        std::sort(sorted_copy.begin(), sorted_copy.end(), fn_comp);

        const auto len = sorted_copy.size();
        const auto value = sorted_copy.at(len / 2);
        if (sorted_copy == manual) {
            total_pt1 += std::stoi(value);
        } else {
            total_pt2 += std::stoi(value);
        }
    }

    std::clog << "Part 1: " << total_pt1 << "\n";
    std::clog << "Part 2: " << total_pt2 << "\n";

    return 0;
}
