#ifndef _STRONK_CODE_GENERATOR_H
#define _STRONK_CODE_GENERATOR_H
#include "instruction.h"
#include "value.h"

using Bytecode = std::vector<Instr>;
using ValueArray = std::vector<Value>;

class CodeGenerator {
private:
    Bytecode bytecode_;
    ValueArray constant_pool_;
public:
    CodeGenerator() = default;
    void AddInstruction(const Instr &instr);
    void AddConstantInstruction(Value val);
    auto Size() -> size_t;
    void DissasembleCode();
    auto GetCode() -> Bytecode;
};

#endif // _STRONK_CODE_GENERATOR_H