#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

#define COMPUTE_PERF 0

#if COMPUTE_PERF
#include <chrono>
using namespace std::chrono;

#define INTERPRET_WITH_PERF(source) \
    auto start = std::chrono::high_resolution_clock::now(); \
\
    InterpretResult result = vm.interpret(source); \
\
    auto stop = std::chrono::high_resolution_clock::now(); \
    auto duration = duration_cast<microseconds>(stop - start); \
    std::cout << duration.count() / 1000.0 << " ms\n";
#endif

// Starts reading from standard input as a REPL and
// compiles + interprets (the bytecode) of each line.
static void repl(VirtualMachine &vm) {
    std::string line;
    for (;;) {
        std::cout << "> ";

        if (!std::getline(std::cin, line)) {
            std::cout << "\n";
            break;
        }

#if COMPUTE_PERF
        INTERPRET_WITH_PERF(line)
#else
        InterpretResult result = vm.interpret(line);
#endif

    }
}

// Reads an entire file and feeds contents to VM to
// compiler and interpret it.
static void run_file(VirtualMachine &vm, std::string path) {
    // Turn file into a string.
    std::ifstream istream(path);

    if (!istream.is_open()) {
        std::cerr << "File does not exist" << "\n";
        exit(74);
    }
    
    std::stringstream buffer;
    buffer << istream.rdbuf();

    std::string source = buffer.str();

    // Interpret result and throw errors as necessary.
#if COMPUTE_PERF
    INTERPRET_WITH_PERF(source)
#else
    InterpretResult result = vm.interpret(source);
#endif

    if (result == INTERPRET_COMPILE_ERROR) exit(65);
    if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(int argc, const char *argv[]) {
    VirtualMachine vm;

    if (argc == 1) {
        repl(vm);
    } else if (argc == 2) {
        run_file(vm, argv[1]);
    } else {
        std::cerr << "Usage: stronk [path]\n";
        exit(64);
    }

    return 0;
}