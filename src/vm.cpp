#include <iostream>
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
        print_stack(stack);

        int offset = std::distance(_chunk.get_code().begin(), ip);
        disassemble_instruction(_chunk, offset);
#endif
        uint8_t instr;
        Value constant;

        // Read a byte from _chunk, which ip points to.
        switch (instr = *ip++) {
            case OP_RETURN:
                print_value(stack.top());
                stack.pop();
                std::cout << "\n";
                return INTERPRET_OK;
            case OP_CONSTANT:
                constant = _chunk.get_constant(*ip++);
                stack.push(constant);
                break;
            case OP_CONSTANT_LONG:
                int left = *ip++;
                int mid = *ip++;
                int right = *ip++;
                constant = _chunk.get_constant((left << 16) + (mid << 8) + right);
                stack.push(constant);
                break;
            // TODO: add OP_CONST_LONG instruction
        }
    }
}