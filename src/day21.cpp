#include <format>
#include <iostream>
#include <print>
#include <unordered_map>
#include <vector>

std::vector<std::string> read_input(std::istream &is) {
    std::vector<std::string> codes;
    std::string line;
    while (std::getline(is, line)) {
        codes.push_back(line);
    }
    return codes;
}

struct position {
    std::size_t row;
    std::size_t col;
    constexpr auto operator<=>(const position &other) const = default;
    std::size_t distance_to(const position &other) const {
        const auto delta_row = row > other.row ? row - other.row : other.row - row;
        const auto delta_col = col > other.col ? col - other.col : other.col - col;
        return delta_row + delta_col;
    }
};

template <> struct std::formatter<position> : std::formatter<std::string> {
    auto format(const position &pos, std::format_context &ctx) const {
        return std::formatter<std::string>::format(std::format("({}, {})", pos.row, pos.col), ctx);
    }
};

static const std::unordered_map<char, position> numpad = {
    {'7', {0uz, 0uz}}, {'8', {0uz, 1uz}}, {'9', {0uz, 2uz}}, {'4', {1uz, 0uz}},
    {'5', {1uz, 1uz}}, {'6', {1uz, 2uz}}, {'1', {2uz, 0uz}}, {'2', {2uz, 1uz}},
    {'3', {2uz, 2uz}}, {'0', {3uz, 1uz}}, {'A', {3uz, 2uz}},
};

static const std::unordered_map<char, position> dirpad = {
    {'^', {0uz, 1uz}}, {'A', {0uz, 2uz}}, {'<', {1uz, 0uz}}, {'v', {1uz, 1uz}}, {'>', {1uz, 2uz}},
};

struct empty {};

std::variant<empty, std::string, std::pair<std::string, std::string>>
get_button_sequences(position start, position target) {
    std::string row_move;
    if (start.row > target.row) {
        row_move = std::string(start.row - target.row, '^');
    } else {
        row_move = std::string(target.row - start.row, 'v');
    }

    std::string col_move;
    if (start.col > target.col) {
        col_move = std::string(start.col - target.col, '<');
    } else {
        col_move = std::string(target.col - start.col, '>');
    }

    const auto seq0 = row_move + col_move;
    const auto seq1 = col_move + row_move;

    if (seq0.empty()) {
        return empty{};
    }
    if (seq0 == seq1) {
        return seq0;
    }
    return std::pair<std::string, std::string>{seq0, seq1};
}

bool is_valid_button_sequence(position pos, const std::string &sequence, const position &gap) {
    for (const auto ch : sequence) {
        switch (ch) {
        case '^':
            --pos.row;
        case '>':
            ++pos.col;
        case 'v':
            ++pos.row;
        case '<':
            --pos.col;
        }
        if (pos == gap) {
            return false;
        }
    }
    return true;
}

bool is_valid_button_sequence_numpad(position pos, const std::string &sequence) {
    return is_valid_button_sequence(pos, sequence, {3uz, 0uz});
}

bool is_valid_button_sequence_dirpad(const position &pos, const std::string &sequence) {
    return is_valid_button_sequence(pos, sequence, {0uz, 0uz});
}

int main() {
    const auto codes = read_input(std::cin);

    const auto code = codes.front();
    std::println("Code: {}", code);

    std::vector<std::vector<std::string>> key_sequences;

    auto pos = numpad.at('A');
    for (const auto ch : code) {
        auto target_pos = numpad.at(ch);
        const auto seqs = get_button_sequences(pos, target_pos);

        std::println("Char: {}", ch);

        if (const auto seq_ptr = std::get_if<empty>(&seqs); seq_ptr) {
            std::println("  No button presses needed");
        }
        if (const auto seq_ptr = std::get_if<std::string>(&seqs); seq_ptr) {
            const auto valid = is_valid_button_sequence_numpad(pos, *seq_ptr);
            std::println("  Sequence 0: {}, valid: {}", *seq_ptr, valid);
        }
        if (const auto seq_ptr = std::get_if<std::pair<std::string, std::string>>(&seqs); seq_ptr) {
            const auto valid0 = is_valid_button_sequence_numpad(pos, seq_ptr->first);
            const auto valid1 = is_valid_button_sequence_numpad(pos, seq_ptr->second);
            std::println("  Sequence 0: {}, valid: {}", seq_ptr->first, valid0);
            std::println("  Sequence 1: {}, valid: {}", seq_ptr->second, valid1);
        }
        pos = target_pos;
    }

    /*Code: 029A*/
    /*Char: 0*/
    /*  Sequence 0: <, valid: true*/
    /*Char: 2*/
    /*  Sequence 0: ^, valid: true*/
    /*Char: 9*/
    /*  Sequence 0: ^^>, valid: true*/
    /*  Sequence 1: >^^, valid: true*/
    /*Char: A*/
    /*  Sequence 0: vvv, valid: true*/

    std::vector<std::string> a = {"<", "^", "^^>", ">^^", "vvv"};
    std::vector<std::string> b = {"<", "^", ">^^", ">^^", "vvv"};

    return 0;
}
