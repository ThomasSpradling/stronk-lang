#include <iostream>
#include "debug.h"
#include "vm.h"

// Utility method for adding to VM stack.
void VirtualMachine::StackPush(Value val) {
    if (stack.size() >= STACK_MAX) {
        throw std::out_of_range("Stack overflow");
    }
    stack.push(val);
}

// Utility method for popping from VM stack.
auto VirtualMachine::StackPop() -> Value {
    Value res = stack.top();
    stack.pop();
    return res;
}

// Interprets the compiled source.
auto VirtualMachine::Interpret(const std::string_view source) -> InterpretResult {
    _compiler.Compile(source);
    return INTERPRET_OK;

    // _chunk = chunk;
    // ip = _chunk.get_code().begin();
    // return run();
}

// Runs the virtual machine by checking each instruction code.
auto VirtualMachine::Run() -> InterpretResult {

// Messy hack to make same task simpler.
#define BINARY_OP(op) \
    do { \
        double b = StackPop(); \
        stack.top() = stack.top() op b; \
    } while (false)


    for (;;) {
#if DEBUG_TRACE_EXECUTION
        print_stack(stack);

        int offset = std::distance(_chunk.get_code().begin(), ip);
        disassemble_instruction(_chunk, offset);
#endif
        // uint8_t instr;
        Value constant;
        int left;
        int mid;
        int right;

        // Read a byte from _chunk, which ip points to.
        switch (*ip++) {
            case OP_CONSTANT:
                constant = _chunk.GetConstant(*ip++);
                StackPush(constant);
                break;
            case OP_CONSTANT_LONG:
                left = *ip++;
                mid = *ip++;
                right = *ip++;
                constant = _chunk.GetConstant((left << 16) + (mid << 8) + right);
                StackPush(constant);
                break;
            case OP_ADD: BINARY_OP(+); break;
            case OP_SUBTRACT: BINARY_OP(-); break;
            case OP_MULTIPLY: BINARY_OP(*); break;
            case OP_DIVIDE: BINARY_OP(/); break;
            case OP_NEGATE:
                stack.top() *= -1;
                break;
            case OP_RETURN:
                PrintValue(StackPop());
                std::cout << "\n";
                return INTERPRET_OK;
        }
    }
#undef BINARY_OP
}