#include "frontend/code_generator.h"
#include <iostream>

namespace stronk {
// Adds an instruction.
void CodeGenerator::AddInstruction(const std::shared_ptr<Instr> &instr) {
    bytecode_.push_back(instr);
}

// Utility method for adding value to the constant
// pool and an instruction that references that constant.
void CodeGenerator::AddConstantInstruction(Address &dest, const ConstantPool::ConstantValue &value, int line, int pos) {
    AddInstruction(std::make_shared<ConstInstr>(dest, constant_pool_.AddConstant(value), line, pos));
}

// Gets the number of instructions.
auto CodeGenerator::Size() -> size_t {
    return bytecode_.size();
}

// Prints each instruction.
void CodeGenerator::DissasembleCode() {
    for (const auto &instr : bytecode_) {
        std::cout << instr->line_ << " " << instr->ToString() << "\n";
    }
}

// Bytecode getter method.
auto CodeGenerator::GetCode() -> Bytecode {
    return bytecode_;
}

} // namespace "stronk"