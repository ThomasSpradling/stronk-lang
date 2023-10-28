#ifndef _STRONK_VM_H
#define _STRONK_VM_H

#include "chunk.h"

enum InterpretResult {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
};

class VirtualMachine {
private:
    Chunk _chunk;
    std::vector<uint8_t>::iterator ip;
public:
    VirtualMachine () {}

    InterpretResult interpret(Chunk &chunk);
    InterpretResult run();
};



#endif // _STRONK_VM_H