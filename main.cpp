#include "common.h"
#include <iostream>
#include <iomanip>
#include "chunk.h"
#include "debug.h"

int main(int argc, const char *argv[]) {
    Chunk chunk;

    chunk.add_constant_chunk(53, 1);
    chunk.add_constant_chunk(23, 2);
    chunk.add_constant_chunk(1012, 3);
    
    chunk.add_chunk(OP_RETURN, 2);
    disassemble_chunk(chunk, "test chunk");

    return 0;
}