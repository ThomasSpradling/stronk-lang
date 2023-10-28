#include <iostream>
#include <iomanip>
#include "debug.h"
#include "value.h"

// Prints an instruction that only has an associated name.
auto SimpleInstruction(std::string_view name, int offset) -> int {
    std::cout << name << "\n";
    return offset + 1;
}

// Prints the an instruction and the following bytecode part.
// Best suitable for OP_CONSTANT which take up two
// bytecodes to form.
auto ConstantInstruction(std::string_view name, const Chunk &chunk, int offset) -> int {
    int constant = chunk.GetChunk(offset + 1);
    std::cout << std::left << std::setw(16) << std::setfill(' ') << name << std::right << std::setw(3) << std::setfill('0') << constant << " '";
    PrintValue(chunk.GetConstant(constant));
    std::cout << "'\n";
    return offset + 2;
}

// Prints an instruction along with the next three bytecodes.
// Best suitable for OP_CONSTANT_LONG which takes up four
// bytecodes to form.
auto ConstantLongInstruction(std::string_view name, const Chunk &chunk, int offset) -> int {
    int left = chunk.GetChunk(offset + 1);
    int mid = chunk.GetChunk(offset + 2);
    int right = chunk.GetChunk(offset + 3);
    int constant = (left << 16) + (mid << 8) + right;
    std::cout << std::left << std::setw(21) << std::setfill(' ') << name << std::right << std::setw(8) << std::setfill('0') << constant << " '";
    PrintValue(chunk.GetConstant(constant));
    std::cout << "'\n";
    return offset + 4;
}

// Prints a single instruction (as given by offset) in a
// human-friendly format.
auto DisassembleInstruction(const Chunk &chunk, int offset) -> int {
    std::cout << std::setw(4) << std::setfill('0') << offset << " ";

    if (offset > 0 && chunk.GetLine(offset) == chunk.GetLine(offset - 1)) {
        std::cout << "   | ";
    } else {
        std::cout << std::setw(4) << chunk.GetLine(offset) << " ";
    }

    uint8_t instr = chunk.GetChunk(offset);

    switch (instr) {
        case OP_CONSTANT:
            return ConstantInstruction("OP_CONSTANT", chunk, offset);
        case OP_CONSTANT_LONG:
            return ConstantLongInstruction("OP_CONSTANT_LONG", chunk, offset);
        case OP_ADD:
            return SimpleInstruction("OP_ADD", offset);
        case OP_SUBTRACT:
            return SimpleInstruction("OP_SUBTRACT", offset);
        case OP_MULTIPLY:
            return SimpleInstruction("OP_MULTIPLY", offset);
        case OP_DIVIDE:
            return SimpleInstruction("OP_DIVIDE", offset);
        case OP_NEGATE:
            return SimpleInstruction("OP_NEGATE", offset);
            
        case OP_RETURN:
            return SimpleInstruction("OP_RETURN", offset);
        default:
            std::cout << "Unknown opcode " << instr;
            return offset + 1;
    }
}

// Prints an entire chunk of instructions in a human-friendly
// format.
void DisassembleChunk(const Chunk &chunk, std::string_view name) {
    std::cout << "== " << name << " ==\n";

    for (int offset = 0; offset < chunk.Size();) {
        offset = DisassembleInstruction(chunk, offset);
    }
}

// Prints a stack of Values. Best used for printing the VM stack
// in a human-friendly way.
void PrintStack(std::stack<Value> &stack) {
    std::cout << "          ";
    if (stack.empty()) {
        std::cout << "<empty stack>" << "\n";
        return;
    }
    std::stack<Value> temp = stack;
    while (!temp.empty()) {
        std::cout << "[ ";
        PrintValue(temp.top());
        std::cout << " ]";
        temp.pop();
    }
    std::cout << "\n";
}