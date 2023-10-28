#include "chunk.h"
#include <iostream>

void Chunk::add_chunk(uint8_t item, int line) {
    code.push_back(item);

    if (lines.size() > 0 && lines[lines.size() - 1].first == line) {
        lines[lines.size() - 1].second++;
    } else {
        lines.push_back(std::make_pair(line, 1));
    }
}
auto Chunk::add_constant(Value val) -> int {
    constants.push_back(val);
    return constants.size() - 1;
}

// Adds a constant instruction.
void Chunk::add_constant_chunk(Value val, int line) {
    if (constants.size() < 256) {
       int constant = add_constant(val);
       add_chunk(OP_CONSTANT, line);
       add_chunk(constant, line);
    } else if (constants.size() < (1 << 25) - 1) {
        // Handle for large number of constants
        int constant = add_constant(val);

        add_chunk(OP_CONSTANT_LONG, line);
        add_chunk((constant >> 16) & 0xFF, line);
        add_chunk((constant >> 8) & 0xFF, line);
        add_chunk(constant & 0xFF, line);
    } else {
        throw "Constant pool overflow!";
    }
}

auto Chunk::get_chunk(int offset) const -> uint8_t {
    if (offset < 0 || offset >= code.size()) {
        throw "Out of bound chunk";
    }
    return code[offset];
}
auto Chunk::get_line(int offset) const -> int {
    if (offset < 0 || offset >= code.size()) {
        throw "Out of bound line";
    }
    int i = 0;
    for (const auto [line, count] : lines) {
        if (offset < i + count) {
            return line;
        }
        i += count;
    }
    return -1;
}
auto Chunk::get_constant(int constant) const -> Value {
    if (constant < 0 || constant >= constants.size()) {
        throw "Out of bound constant";
    }
    return constants[constant];
}

auto Chunk::size() const -> size_t {
    return code.size();
}