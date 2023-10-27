#include <iostream>
#include <iomanip>
#include "debug.h"
#include "value.h"

auto __simple_instruction(std::string name, int offset) -> int {
    std::cout << name << "\n";
    return offset + 1;
}

auto __constant_instruction(std::string name, const Chunk &chunk, int offset) -> int {
    int constant = chunk.get_chunk(offset + 1);
    std::cout << std::left << std::setw(16) << std::setfill(' ') << name << std::right << std::setw(3) << std::setfill('0') << constant << " '";
    print_value(chunk.get_constant(constant));
    std::cout << "'\n";
    return offset + 2;
}

auto __constant_long_instruction(std::string name, const Chunk &chunk, int offset) -> int {
    int left = chunk.get_chunk(offset + 1);
    int mid = chunk.get_chunk(offset + 2);
    int right = chunk.get_chunk(offset + 3);
    int constant = (left << 16) + (mid << 8) + right;
    std::cout << std::left << std::setw(21) << std::setfill(' ') << name << std::right << std::setw(8) << std::setfill('0') << constant << " '";
    print_value(chunk.get_constant(constant));
    std::cout << "'\n";
    return offset + 4;
}

auto __disassemble_instruction(const Chunk &chunk, int offset) -> int {
    std::cout << std::setw(4) << std::setfill('0') << offset << " ";

    if (offset > 0 && chunk.get_line(offset) == chunk.get_line(offset - 1)) {
        std::cout << "   | ";
    } else {
        std::cout << std::setw(4) << chunk.get_line(offset) << " ";
    }

    uint8_t instr = chunk.get_chunk(offset);

    switch (instr) {
        case OP_CONSTANT:
            return __constant_instruction("OP_CONSTANT", chunk, offset);
        case OP_CONSTANT_LONG:
            return __constant_long_instruction("OP_CONSTANT_LONG", chunk, offset);
        case OP_RETURN:
            return __simple_instruction("OP_RETURN", offset);
        default:
            std::cout << "Unknown opcode " << instr;
            return offset + 1;
    }
}

void disassemble_chunk(const Chunk &chunk, std::string_view name) {
    std::cout << "== " << name << " ==\n";

    for (int offset = 0; offset < chunk.size();) {
        offset = __disassemble_instruction(chunk, offset);
    }
}