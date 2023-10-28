#include <iostream>
#include <iomanip>

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char *argv[]) {
    VirtualMachine vm;
    Chunk chunk;

    chunk.add_constant_chunk(53, 1);
    chunk.add_constant_chunk(23, 2);
    chunk.add_constant_chunk(1012, 3);
    
    chunk.add_chunk(OP_RETURN, 2);
    disassemble_chunk(chunk, "test chunk");

    vm.interpret(chunk);

    return 0;

    // std::vector<uint8_t> vec { 1, 2, 3, 4 };

    // std::vector<uint8_t>::iterator ip = vec.begin();

    // for (; ip != vec.end(); ip++) {
    //     std::cout << "entry of arr: " << int(*ip) << "\n";
    // }
}