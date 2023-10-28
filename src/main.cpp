#include <iostream>
#include <iomanip>

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

#define COMPUTE_PERF 0

#if COMPUTE_PERF
#include <chrono>
using namespace std::chrono;
#endif

int main(int argc, const char *argv[]) {
    VirtualMachine vm;
    Chunk chunk;

    chunk.add_constant_chunk(10, 1);
    chunk.add_constant_chunk(20, 1);

    for (int i = 0; i < 100000; ++i) {
        chunk.add_chunk(OP_ADD, 1);

        chunk.add_constant_chunk(5, 1);
        chunk.add_chunk(OP_SUBTRACT, 1);

        chunk.add_constant_chunk(5, 1);
        chunk.add_chunk(OP_DIVIDE, 1);

        chunk.add_constant_chunk(2, 1);
        chunk.add_chunk(OP_MULTIPLY, 1);

        chunk.add_constant_chunk(20, 1);
    }

    chunk.add_chunk(OP_RETURN, 1);

    disassemble_chunk(chunk, "test chunk");

#if COMPUTE_PERF
    auto start = std::chrono::high_resolution_clock::now();
#endif

    vm.interpret(chunk);

#if COMPUTE_PERF
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    std::cout << duration.count() / 1000.0 << " ms\n";
#endif

    return 0;
}