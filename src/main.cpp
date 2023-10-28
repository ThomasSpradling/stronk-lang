#include <iostream>
#include <iomanip>

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char *argv[]) {
    VirtualMachine vm;
    Chunk chunk;

    chunk.add_constant_chunk(1.2, 123);
    chunk.add_constant_chunk(3.4, 123);
    chunk.add_chunk(OP_ADD, 123);

    chunk.add_constant_chunk(5.6, 123);

    chunk.add_chunk(OP_DIVIDE, 123);

    chunk.add_chunk(OP_NEGATE, 123);
    chunk.add_chunk(OP_RETURN, 123);
    disassemble_chunk(chunk, "test chunk");

    vm.interpret(chunk);

    return 0;
}