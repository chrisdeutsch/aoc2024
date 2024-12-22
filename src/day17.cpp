#include <absl/strings/str_join.h>
#include <absl/strings/str_split.h>
#include <iostream>
#include <print>
#include <regex>
#include <set>
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
        std::println("A = {} B = {} C = {} IP = {}", ps.reg_a, ps.reg_b, ps.reg_c, ps.ip);

        if (out) {
            if (n_out >= state.program.size() || *out != state.program[n_out]) {
                return false;
            }
            ++n_out;
        }
    }
}

int main() {
    const auto p = read_input(std::cin);

    auto state_pt1 = p;
    const auto output = execute_until_halt(state_pt1);
    std::println("Part 1: {}", absl::StrJoin(output, ","));

    std::set<program_state> not_reproducing;
    std::size_t steps_until_reproduced = 0;
    for (std::size_t reg_a = 0; reg_a < 100000; ++reg_a) {
        auto state_pt2 = p;
        state_pt2.reg_a = reg_a;

        if (program_reproduces_itself(state_pt2)) {
            steps_until_reproduced = reg_a;
            break;
        }
    }
    std::println("Part 2: {}", steps_until_reproduced);

    return 0;
}
