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
#define BINARY_OP(op) \
    do { \
        double b = stack.top(); \
        stack.pop(); \
        double a = stack.top(); \
        stack.pop(); \
        stack.push(a op b); \
    } while (false)


    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        print_stack(stack);

        int offset = std::distance(_chunk.get_code().begin(), ip);
        disassemble_instruction(_chunk, offset);
#endif
        uint8_t instr;
        Value constant;
        int left, mid, right;

        // Read a byte from _chunk, which ip points to.
        switch (instr = *ip++) {
            case OP_CONSTANT:
                constant = _chunk.get_constant(*ip++);
                stack.push(constant);
                break;
            case OP_CONSTANT_LONG:
                left = *ip++;
                mid = *ip++;
                right = *ip++;
                constant = _chunk.get_constant((left << 16) + (mid << 8) + right);
                stack.push(constant);
                break;
            case OP_ADD: BINARY_OP(+); break;
            case OP_SUBTRACT: BINARY_OP(-); break;
            case OP_MULTIPLY: BINARY_OP(*); break;
            case OP_DIVIDE: BINARY_OP(/); break;
            case OP_NEGATE:
                constant = stack.top();
                stack.pop();
                stack.push(-constant);
                break;
            case OP_RETURN:
                print_value(stack.top());
                stack.pop();
                std::cout << "\n";
                return INTERPRET_OK;
            // TODO: add OP_CONST_LONG instruction
        }
    }
#undef BINARY_OP
}