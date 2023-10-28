#include <iostream>
#include "common.h"
#include "debug.h"
#include "vm.h"

InterpretResult VirtualMachine::interpret(Chunk &chunk) {
    _chunk = chunk;
    ip = _chunk.get_code().begin();
    return run();
}

// Runs the virtual machine by checking each instruction code.
InterpretResult VirtualMachine::run() {
// (TODO) Potentials for dispatching optimizations exist here

    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        int offset = std::distance(_chunk.get_code().begin(), ip);
        disassemble_instruction(_chunk, offset);
#endif
        uint8_t instr;

        // Read a byte from _chunk, which ip points to.
        switch (instr = *ip++) {
            case OP_RETURN:
                return INTERPRET_OK;
            case OP_CONSTANT:
                Value constant = _chunk.get_constant(*ip++);
                print_value(constant);
                std::cout << "\n";
                break;
            // TODO: add OP_CONST_LONG instruction
        }
    }
}