#include "frontend/code_generator.h"
#include <iostream>

// Adds an instruction.
void CodeGenerator::AddInstruction(const std::shared_ptr<Instr> &instr) {
    bytecode_.push_back(instr);
}

// Utility method for adding value to the constant
// pool and an instruction that references that constant.
void CodeGenerator::AddConstantInstruction(Address &dest, const Value &value, int line, int pos) {
    AddInstruction(std::make_shared<ConstInstr>(dest, AddConstant(value), line, pos));
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

// Adds a constant to constant pool and returns its index.
auto CodeGenerator::AddConstant(const Value &val) -> int {
    constant_pool_.push_back(val);
    return constant_pool_.size() - 1;
}