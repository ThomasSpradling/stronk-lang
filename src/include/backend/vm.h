#ifndef _STRONK_VM_H
#define _STRONK_VM_H

#include "frontend/code_generator.h"

namespace stronk {

class VirtualMachine {
private:
    Bytecode bytecode_;
public:
    VirtualMachine() = default;
    auto Interpret(Bytecode &bytecode) -> bool;
};

} // namespace "stronk"

#endif // _STRONK_VM_H