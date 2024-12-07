#include <iostream>
#include <sstream>
#include <vector>

using report = std::vector<int>;

std::vector<report> get_reports(std::istream &istream) {
    std::vector<report> reports;

    std::string line;
    while (std::getline(istream, line)) {
        std::stringstream ss(line);
        report r;

        unsigned level;
        while (ss >> level) {
            r.push_back(level);
        }

        reports.emplace_back(std::move(r));
    }

    return reports;
}

bool report_is_safe(const report &r) {
    int prev_diff = 0;

    auto it1 = r.cbegin();
    auto it2 = r.cbegin() + 1;
    while (it2 != r.cend()) {
        const auto diff = *it2 - *it1;

        // Opposite sign -> non-monotonic
        if (prev_diff * diff < 0) {
            return false;
        }

        const auto pass_diff_constraint = diff != 0 && -3 <= diff && diff <= 3;
        if (!pass_diff_constraint) {
            return false;
        }

        it1++;
        it2++;
        prev_diff = diff;
    }

    return true;
}

int main(int argc, char *argv[]) {
    const auto reports = get_reports(std::cin);

    std::size_t num_safe_reports = 0;
    for (const auto &report : reports) {
        if (report_is_safe(report)) {
            num_safe_reports++;
        }
    }

    std::clog << "Part 1: " << num_safe_reports << "\n";

    return 0;
}
