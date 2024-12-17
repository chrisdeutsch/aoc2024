#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

std::vector<int> read_input(std::istream &istream) {
    std::string line;
    if (istream) {
        std::getline(istream, line);
    } else {
        throw std::runtime_error("error reading stream");
    }
    std::vector<int> v;
    std::transform(line.cbegin(), line.cend(), std::back_inserter(v),
                   [](char c) { return static_cast<int>(c - '0'); });
    return v;
}

struct block {
    std::size_t file_id;
    bool free;
};

void print_blocks(const std::vector<block> &blocks) {
    for (const auto &block : blocks) {
        if (block.free) {
            std::clog << ".";
        } else {
            std::clog << block.file_id;
        }
    }
    std::clog << "\n";
}

std::vector<block> disk_map_to_blocks(const std::vector<int> &disk_map) {
    const auto num_files = disk_map.size() / 2;
    const auto total_size =
        std::accumulate(disk_map.cbegin(), disk_map.cend(), static_cast<std::size_t>(0));

    std::vector<block> blocks;
    blocks.reserve(total_size);

    bool is_file = true;
    std::size_t file_id = 0;
    for (const auto num_blocks : disk_map) {
        if (is_file) {
            for (std::size_t i = 0; i < num_blocks; ++i) {
                blocks.push_back({
                    .file_id = file_id,
                    .free = false,
                });
            }
            ++file_id;
        } else {
            for (std::size_t i = 0; i < num_blocks; ++i) {
                blocks.push_back({
                    .file_id = 0,
                    .free = true,
                });
            }
        }
        // Alternate between file and free
        is_file = !is_file;
    }

    return blocks;
}

std::vector<block> compact_disk(std::vector<block> blocks) {
    std::size_t i = 0, j = blocks.size() - 1;
    while (i < j) {
        // Find first free
        while (!blocks[i].free) {
            ++i;
        }
        // Find first non-free from back
        while (blocks[j].free) {
            --j;
        }
        if (i < j) {
            std::swap(blocks[i], blocks[j]);
        }
    }
    return blocks;
}

std::size_t checksum(const std::vector<block> &blocks) {
    std::size_t total = 0;
    for (std::size_t i = 0; i < blocks.size(); ++i) {
        if (!blocks[i].free) {
            total += i * blocks[i].file_id;
        }
    }
    return total;
}

// Length file - length free space
int main() {
    const auto input = read_input(std::cin);
    const auto blocks = disk_map_to_blocks(input);
    const auto blocks_compacted = compact_disk(blocks);

    std::clog << "Part 1: " << checksum(blocks_compacted) << "\n";

    return 0;
}
