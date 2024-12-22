#include <absl/strings/str_split.h>
#include <iostream>
#include <print>
#include <regex>
#include <vector>

struct program_state {
    std::size_t reg_a = 0;
    std::size_t reg_b = 0;
    std::size_t reg_c = 0;
    std::vector<uint8_t> program;
    std::size_t ip = 0;
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
    if (0 <= operand && operand <= 3) {
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

void execute_instruction(opcode oc, uint8_t operand, program_state &state) {
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
        std::print("{},", combo_operand.value() % 8);
        break;
    case opcode::bdv:
        state.reg_b = state.reg_a >> combo_operand.value();
        break;
    case opcode::cdv:
        state.reg_c = state.reg_a >> combo_operand.value();
        break;
    }
}

void execute_until_halt(program_state &state) {
    while (true) {
        if (state.ip >= state.program.size()) { // halt program
            std::print("\b \b\n");              // removes trailing space
            return;
        }

        const auto oc = static_cast<opcode>(state.program[state.ip++]);
        const auto operand = state.program[state.ip++];

        execute_instruction(oc, operand, state);
    }
}

int main() {
    auto p = read_input(std::cin);
    std::print("Part 1: ");
    execute_until_halt(p);

    return 0;
}
