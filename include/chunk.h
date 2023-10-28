#ifndef _STRONK_CHUNK_H
#define _STRONK_CHUNK_H

#include <vector>
#include <map>
#include "value.h"

enum OpCode {
    OP_CONSTANT,
    OP_CONSTANT_LONG,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE,
    OP_RETURN,
};

class Chunk {
private:
    std::vector<uint8_t> code;
    ValueArray constants; // constant pool

    // Utilizing run-length encoding: https://en.wikipedia.org/wiki/Run-length_encoding
    std::vector<std::pair<int, int>> lines;
public:
    void AddChunk(uint8_t item, int line);
    auto AddConstant(Value val) -> int;
    void AddConstantChunk(Value val, int line);
    auto GetChunk(int offset) const -> uint8_t;
    auto GetLine(int offset) const -> int;
    auto GetConstant(int constant) const -> Value;

    auto Size() const -> size_t;
    
    auto GetCode() -> std::vector<uint8_t> & {
        return code;
    }
    auto GetConstants() -> ValueArray & {
        return constants;
    }
};

#endif // _STRONK_CHUNK_H