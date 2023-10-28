#include <iostream>
#include "debug.h"
#include "vm.h"

void VirtualMachine::__stack_push(Value val) {
    if (stack.size() >= STACK_MAX) {
        throw std::out_of_range("Stack overflow");
    }
    stack.push(val);
}
Value VirtualMachine::__stack_pop() {
    Value res = stack.top();
    stack.pop();
    return res;
}

InterpretResult VirtualMachine::interpret(Chunk &chunk) {
    _chunk = chunk;
    ip = _chunk.get_code().begin();
    return run();
}

// Runs the virtual machine by checking each instruction code.
InterpretResult VirtualMachine::run() {
#define BINARY_OP(op) \
    do { \
        double b = __stack_pop(); \
        double a = __stack_pop(); \
        __stack_push(a op b); \
    } while (false)


    for (;;) {
#if DEBUG_TRACE_EXECUTION
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
                __stack_push(constant);
                break;
            case OP_CONSTANT_LONG:
                left = *ip++;
                mid = *ip++;
                right = *ip++;
                constant = _chunk.get_constant((left << 16) + (mid << 8) + right);
                __stack_push(constant);
                break;
            case OP_ADD: BINARY_OP(+); break;
            case OP_SUBTRACT: BINARY_OP(-); break;
            case OP_MULTIPLY: BINARY_OP(*); break;
            case OP_DIVIDE: BINARY_OP(/); break;
            case OP_NEGATE:
                stack.top() *= -1;
                break;
            case OP_RETURN:
                print_value(__stack_pop());
                std::cout << "\n";
                return INTERPRET_OK;
        }
    }
#undef BINARY_OP
}