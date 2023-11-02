#include "vm.h"

// Interprets the given bytecode.
auto VirtualMachine::Interpret(Bytecode &bytecode) -> bool {
    bytecode_ = bytecode;
    return false;
}