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
    auto it1 = r.cbegin();
    auto it2 = r.cbegin() + 1;

    int prev_diff = 0;
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

bool report_is_safe_with_removal(const report &r) {
    const report omit_first(r.cbegin() + 1, r.cend());
    if (report_is_safe(omit_first)) {
        return true;
    }

    auto it1 = r.cbegin();
    auto it2 = r.cbegin() + 1;

    int prev_diff = 0;
    while (it2 != r.cend()) {
        const auto diff = *it2 - *it1;

        // Opposite sign -> non-monotonic
        if (prev_diff * diff < 0) {
            report r1;
            std::copy(r.cbegin(), it1, std::back_inserter(r1));
            std::copy(it1 + 1, r.cend(), std::back_inserter(r1));

            report r2;
            std::copy(r.cbegin(), it2, std::back_inserter(r2));
            std::copy(it2 + 1, r.cend(), std::back_inserter(r2));

            return report_is_safe(r1) || report_is_safe(r2);
        }

        const auto pass_diff_constraint = diff != 0 && -3 <= diff && diff <= 3;
        if (!pass_diff_constraint) {
            report r1;
            std::copy(r.cbegin(), it1, std::back_inserter(r1));
            std::copy(it1 + 1, r.cend(), std::back_inserter(r1));

            report r2;
            std::copy(r.cbegin(), it2, std::back_inserter(r2));
            std::copy(it2 + 1, r.cend(), std::back_inserter(r2));

            return report_is_safe(r1) || report_is_safe(r2);
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
    std::size_t num_safe_reports_with_removal = 0;
    for (const auto &report : reports) {
        if (report_is_safe(report)) {
            num_safe_reports++;
        }
        if (report_is_safe_with_removal(report)) {
            num_safe_reports_with_removal++;
        }
    }

    std::clog << "Part 1: " << num_safe_reports << "\n";
    std::clog << "Part 2: " << num_safe_reports_with_removal << "\n";

    return 0;
}
