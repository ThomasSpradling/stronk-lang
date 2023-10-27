#ifndef _STRONK_CHUNK_H
#define _STRONK_CHUNK_H

#include <vector>
#include <iostream>
#include <map>
#include "value.h"

enum OpCode {
    OP_CONSTANT,
    OP_CONSTANT_LONG,
    OP_RETURN,
};

class Chunk {
private:
    std::vector<uint8_t> code;
    ValueArray constants; // constant pool

    // Utilizing run-length encoding: https://en.wikipedia.org/wiki/Run-length_encoding
    std::vector<std::pair<int, int>> lines;
public:
    void add_chunk(uint8_t item, int line);
    auto add_constant(Value val) -> int;
    void add_constant_chunk(Value val, int line);
    auto get_chunk(int offset) const -> uint8_t;
    auto get_line(int offset) const -> int;
    auto get_constant(uint8_t constant) const -> Value;

    auto size() const -> size_t;
    
    auto get_code() const -> std::vector<uint8_t> {
        return code;
    }
    auto get_constants() const -> ValueArray {
        return constants;
    }
};

#endif // _STRONK_CHUNK_H