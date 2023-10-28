#include <iostream>
#include <iomanip>

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char *argv[]) {
    VirtualMachine vm;
    Chunk chunk;

    for (int i = 0; i < 300; i++) {
        chunk.add_constant_chunk(2 * i, i);
    }
    chunk.add_constant_chunk(23992, 1000);
    
    chunk.add_chunk(OP_RETURN, 2);
    disassemble_chunk(chunk, "test chunk");

    vm.interpret(chunk);

    return 0;
}