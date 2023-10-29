#ifndef _STRONK_VM_H
#define _STRONK_VM_H

#include <stack>
#include "chunk.h"
#include "value.h"
#include "common.h"
#include "compiler.h"

#define STACK_MAX 256

enum InterpretResult {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
};

// Virtual machine for the bytecode produced by the
// compiler.
class VirtualMachine {
private:
    Chunk _chunk;
    std::vector<uint8_t>::iterator ip;
    std::stack<Value> stack;
    void StackPush(Value val);
    auto StackPop() -> Value;
    Compiler _compiler;
public:
    VirtualMachine(const std::string_view source);

    auto Interpret() -> InterpretResult;
    auto Run() -> InterpretResult;
};

#endif // _STRONK_VM_H