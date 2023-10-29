#include <iostream>
#include "chunk.h"

// Adds an instruction to the chunk at line number `line`.
void Chunk::AddChunk(uint8_t item, int line) {
    code.push_back(item);

    if (!lines.empty() && lines[lines.size() - 1].first == line) {
        lines[lines.size() - 1].second++;
    } else {
        lines.emplace_back(line, 1);
    }
}

// Adds a constant to the the constant pool.
auto Chunk::AddConstant(Value val) -> int {
    constants.push_back(val);
    return constants.size() - 1;
}

// A utility method for adding a constant instruction.
// It (1) adds `val` to the constant pool and (2) adds
// the produced index to the chunk. Returns error (false)
// iff there are at least 2^25 constants.
auto Chunk::AddConstantChunk(Value val, int line) -> bool {
    if (constants.size() <= UINT8_MAX) {
       int constant = AddConstant(val);
       AddChunk(OP_CONSTANT, line);
       AddChunk(constant, line);
    } else if (constants.size() <= (1 << 25) - 1) {
        // Handle for large number of constants
        int constant = AddConstant(val);

        AddChunk(OP_CONSTANT_LONG, line);
        AddChunk((constant >> 16) & 0xFF, line);
        AddChunk((constant >> 8) & 0xFF, line);
        AddChunk(constant & 0xFF, line);
    } else {
        return false;
    }
    return true;
}

// Gets an instruction from the chunk at `offset`.
auto Chunk::GetChunk(int offset) const -> uint8_t {
    if (offset < 0 || offset >= code.size()) {
        throw "Out of bound chunk";
    }
    return code[offset];
}

// Gets the line number associated with an offset.
auto Chunk::GetLine(int offset) const -> int {
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

// Gets the constant value associated with the given index.
auto Chunk::GetConstant(int constant) const -> Value {
    if (constant < 0 || constant >= constants.size()) {
        throw "Out of bound constant";
    }
    return constants[constant];
}

// The number of instructions.
auto Chunk::Size() const -> size_t {
    return code.size();
}