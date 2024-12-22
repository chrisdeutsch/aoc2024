#include <absl/strings/str_join.h>
#include <absl/strings/str_split.h>
#include <cmath>
#include <iostream>
#include <print>
#include <ranges>
#include <regex>
#include <vector>

struct program_state {
    std::size_t reg_a = 0;
    std::size_t reg_b = 0;
    std::size_t reg_c = 0;
    std::vector<uint8_t> program;
    std::size_t ip = 0;

    bool operator<(const program_state &other) {
        return std::tie(reg_a, reg_b, reg_c, ip) <
               std::tie(other.reg_a, other.reg_b, other.reg_c, ip);
    }

    bool operator==(const program_state &other) {
        return reg_a == other.reg_a && reg_b == other.reg_b && reg_c == other.reg_c &&
               program == other.program && ip == other.ip;
    }
};

program_state read_input(std::istream &is) {
    program_state state;
    const auto content = std::string{std::istreambuf_iterator<char>(is), {}};

    const std::regex register_pattern(R"(Register A: (\d+)\nRegister B: (\d+)\nRegister C: (\d+))");
    const std::regex program_pattern(R"(Program: ((?:\d+)(?:,\d+)*))");

    if (std::smatch sm; std::regex_search(content.cbegin(), content.cend(), sm, register_pattern)) {
        state.reg_a = std::stoul(sm[1]);
        state.reg_b = std::stoul(sm[2]);
        state.reg_c = std::stoul(sm[3]);
    } else {
        throw std::runtime_error("Could not parse registers");
    }

    if (std::smatch sm; std::regex_search(content.cbegin(), content.cend(), sm, program_pattern)) {
        const std::string program_str = sm[1];
        for (const auto elem : absl::StrSplit(program_str, ",")) {
            state.program.push_back(std::stoul(std::string(elem)));
        };
    } else {
        throw std::runtime_error("Could not parse program");
    }

    return state;
}

enum class opcode : uint8_t {
    adv = 0,
    bxl = 1,
    bst = 2,
    jnz = 3,
    bxc = 4,
    out = 5,
    bdv = 6,
    cdv = 7,
};

std::optional<std::size_t> combo_operand_to_value(uint8_t operand, const program_state &state) {
    if (operand <= 3) {
        return operand;
    } else if (operand == 4) {
        return state.reg_a;
    } else if (operand == 5) {
        return state.reg_b;
    } else if (operand == 6) {
        return state.reg_c;
    }
    return {};
}

std::optional<uint8_t> execute_instruction(opcode oc, uint8_t operand, program_state &state) {
    const auto literal_operand_value = static_cast<std::size_t>(operand);
    const auto combo_operand = combo_operand_to_value(operand, state);

    switch (oc) {
    case opcode::adv:
        state.reg_a = state.reg_a >> combo_operand.value();
        break;
    case opcode::bxl:
        state.reg_b = state.reg_b ^ literal_operand_value;
        break;
    case opcode::bst:
        state.reg_b = combo_operand.value() % 8;
        break;
    case opcode::jnz:
        if (state.reg_a > 0) {
            state.ip = literal_operand_value;
        }
        break;
    case opcode::bxc:
        state.reg_b = state.reg_b ^ state.reg_c;
        break;
    case opcode::out:
        return combo_operand.value() % 8;
        break;
    case opcode::bdv:
        state.reg_b = state.reg_a >> combo_operand.value();
        break;
    case opcode::cdv:
        state.reg_c = state.reg_a >> combo_operand.value();
        break;
    }
    return {};
}

std::vector<uint8_t> execute_until_halt(program_state &state) {
    std::vector<uint8_t> output;

    while (true) {
        if (state.ip >= state.program.size()) { // halt program
            return output;
        }

        const auto oc = static_cast<opcode>(state.program[state.ip++]);
        const auto operand = state.program[state.ip++];

        const auto out = execute_instruction(oc, operand, state);
        if (out) {
            output.push_back(*out);
        }
    }
}

bool program_reproduces_itself(const program_state &state) {
    program_state ps = state;
    std::size_t n_out = 0;

    while (true) {
        if (ps.ip >= ps.program.size()) {
            return n_out == state.program.size();
        }

        const auto oc = static_cast<opcode>(ps.program[ps.ip++]);
        const auto operand = ps.program[ps.ip++];

        const auto out = execute_instruction(oc, operand, ps);
        if (out) {
            if (n_out >= state.program.size() || *out != state.program[n_out]) {
                return false;
            }
            ++n_out;
        }
    }
}

// Program: 2,4,1,3,7,5,0,3,1,5,4,1,5,5,3,0
// 2,4 -> bst A -> b = a % 8
// 1,3 -> bxl 3 -> b = b ^ 3
// 7,5 -> cdv B -> c = a >> b
// 0,3 -> adv 3 -> a = a >> 3
// 1,5 -> bxl 5 -> b = b ^ 5
// 4,1 -> bxc _ -> b = b ^ c
// 5,5 -> out B -> b
// 3,0 -> jnz 0 -> if a != 0 -> goto 0

int main() {
    const auto p = read_input(std::cin);

    auto state_pt1 = p;
    const auto output = execute_until_halt(state_pt1);
    std::println("Part 1: {}", absl::StrJoin(output, ","));
    std::size_t num_digits = 0;

    // Part 2
    // This part is cursed and quite buggy. But it sort of works!
    std::map<uint8_t, std::vector<std::size_t>> output_to_bits;
    for (std::size_t reg_a = 0; reg_a < (1 << 10); ++reg_a) {
        auto state_pt2 = p;
        state_pt2.reg_a = reg_a;
        const auto out = execute_until_halt(state_pt2);
        output_to_bits[out[0]].push_back(reg_a);
    }

    std::vector<std::size_t> candidates;

    const auto first_instruction = p.program[0];
    std::copy(output_to_bits[first_instruction].cbegin(), output_to_bits[first_instruction].cend(),
              std::back_inserter(candidates));

    for (const auto [out_idx, out] : p.program | std::views::enumerate | std::views::drop(1)) {
        const auto &options = output_to_bits[out];

        std::vector<std::size_t> new_candidates;
        while (!candidates.empty()) {
            const auto candidate = candidates.back();
            candidates.pop_back();

            for (const auto opt : options) {
                const auto opt_shifted = opt << (3uz * out_idx);
                const auto mask_shifted = 0b1111111uz << (3uz * out_idx);
                const auto is_match = (candidate & mask_shifted) == (opt_shifted & mask_shifted);
                if (is_match) {
                    new_candidates.push_back(candidate | opt_shifted);
                }
            }
        }
        candidates = new_candidates;
    }

    // There appear to be additional outputs after the last instruction of the program???
    auto filter_good_candidates = [p](const auto candidate) {
        program_state state = p;
        state.reg_a = candidate;
        const auto out = execute_until_halt(state);
        return out == state.program;
    };

    const auto result_pt2 =
        std::ranges::min(candidates | std::views::filter(filter_good_candidates));
    std::println("Part 2: {}", result_pt2);

    return 0;
}
