#ifndef _STRONK_VM_H
#define _STRONK_VM_H

#include "code_generator.h"

class VirtualMachine {
private:
    Bytecode bytecode_;
public:
    VirtualMachine() = default;
    auto Interpret(Bytecode &bytecode) -> bool;
};

#endif // _STRONK_VM_H