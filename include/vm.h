#ifndef _STRONK_VM_H
#define _STRONK_VM_H

#include <stack>
#include "chunk.h"
#include "value.h"
#include "common.h"

#define STACK_MAX 256

enum InterpretResult {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
};

class VirtualMachine {
private:
    Chunk _chunk;
    std::vector<uint8_t>::iterator ip;
    std::stack<Value> stack;
public:
    VirtualMachine () {}

    InterpretResult interpret(Chunk &chunk);
    InterpretResult run();
};



#endif // _STRONK_VM_H