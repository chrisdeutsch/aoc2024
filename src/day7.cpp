#include <absl/strings/str_split.h>
#include <cmath>
#include <iostream>
#include <iterator>

using IntType = long long;

struct equation {
    IntType result;
    std::vector<IntType> operands;
};

std::vector<equation> read_input(std::istream &istream) {
    std::vector<equation> equations;

    std::string line;
    while (istream && std::getline(istream, line)) {
        std::pair<std::string, std::string> split = absl::StrSplit(line, ": ");
        const auto result = std::stoll(split.first);

        std::vector<std::string> operands_str = absl::StrSplit(split.second, " ");
        std::vector<IntType> operands;
        std::transform(operands_str.cbegin(), operands_str.cend(), std::back_inserter(operands),
                       [](auto elem) { return std::stoll(elem); });

        equations.push_back({.result = result, .operands = std::move(operands)});
    }

    return equations;
}

IntType concat_op(IntType lhs, IntType rhs) {
    std::size_t num_digits = 0;
    auto rhs_copy = rhs;
    while (rhs_copy >= 1) {
        rhs_copy /= 10;
        num_digits++;
    }
    return lhs * std::pow(10, num_digits) + rhs;
}

bool eq_ok_rec(const IntType result, const IntType acc, std::vector<IntType>::const_iterator start,
               std::vector<IntType>::const_iterator end) {
    if (start == end) {
        return result == acc;
    }

    const IntType acc_plus = acc + *start;
    const IntType acc_mult = acc * *start;

    return eq_ok_rec(result, acc_plus, start + 1, end) ||
           eq_ok_rec(result, acc_mult, start + 1, end);
}

bool eq_ok_with_concat_rec(const IntType result, const IntType acc,
                           std::vector<IntType>::const_iterator start,
                           std::vector<IntType>::const_iterator end) {
    if (start == end) {
        return result == acc;
    }

    const IntType acc_plus = acc + *start;
    const IntType acc_mult = acc * *start;
    const IntType acc_concat = concat_op(acc, *start);

    return eq_ok_with_concat_rec(result, acc_plus, start + 1, end) ||
           eq_ok_with_concat_rec(result, acc_mult, start + 1, end) ||
           eq_ok_with_concat_rec(result, acc_concat, start + 1, end);
}

bool eq_ok(const equation &eq) {
    return eq_ok_rec(eq.result, eq.operands.front(), eq.operands.cbegin() + 1, eq.operands.cend());
}

bool eq_ok_with_concat(const equation &eq) {
    return eq_ok_with_concat_rec(eq.result, eq.operands.front(), eq.operands.cbegin() + 1,
                                 eq.operands.cend());
}

int main() {
    const auto equations = read_input(std::cin);

    long total_pt1 = 0;
    long total_pt2 = 0;
    for (const auto &eq : equations) {
        if (eq_ok(eq)) {
            total_pt1 += eq.result;
        }
        if (eq_ok_with_concat(eq)) {
            total_pt2 += eq.result;
        }
    }

    std::clog << "Part 1: " << total_pt1 << "\n";
    std::clog << "Part 2: " << total_pt2 << "\n";

    return 0;
}
