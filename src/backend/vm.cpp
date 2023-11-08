#include "backend/vm.h"

namespace stronk {

// Interprets the given bytecode.
auto VirtualMachine::Interpret(Bytecode &bytecode) -> bool {
    bytecode_ = bytecode;
    return false;
}

} // namespace "stronk"
