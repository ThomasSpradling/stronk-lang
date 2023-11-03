#ifndef _STRONK_CODE_GENERATOR_H
#define _STRONK_CODE_GENERATOR_H
#include <memory>
#include "common/instruction.h"
#include "common/value.h"

using Bytecode = std::vector<std::shared_ptr<Instr>>;
using ValueArray = std::vector<Value>;

class CodeGenerator {
private:
    Bytecode bytecode_;
    ValueArray constant_pool_;
    auto AddConstant(const Value &val) -> int;
public:
    CodeGenerator() = default;
    void AddInstruction(const std::shared_ptr<Instr> &instr);
    void AddConstantInstruction(Address &dest, const Value &value, int line, int pos);
    auto Size() -> size_t;
    void DissasembleCode();
    auto GetCode() -> Bytecode;
};

#endif // _STRONK_CODE_GENERATOR_H