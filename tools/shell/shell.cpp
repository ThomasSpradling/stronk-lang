#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "common/common.h"
#include "compiler/compiler.h"
#include "backend/vm.h"

#define COMPUTE_PERF 0

#if COMPUTE_PERF
#include <chrono>
using namespace std::chrono;

#define INTERPRET_WITH_PERF() \
    auto start = std::chrono::high_resolution_clock::now(); \
    auto stop = std::chrono::high_resolution_clock::now(); \
    auto duration = duration_cast<microseconds>(stop - start); \
    std::cout << duration.count() / 1000.0 << " ms\n";
#endif

// Starts reading from standard input as a REPL and
// compiles + interprets (the bytecode) of each line.
static void Repl() {
    // TODO(thomasspradling): Handle REPL scoping.

    std::string line;
    stronk::Compiler compiler;
    stronk::VirtualMachine vm;

    for (;;) {
        std::cout << "> ";

        if (!std::getline(std::cin, line)) {
            std::cout << "\n";
            break;
        }

        if (compiler.Compile(line)) {
            exit(65); // compile time error
        }

        stronk::Bytecode bytecode = compiler.GetBytecode();

        if (vm.Interpret(bytecode)) {
            exit(70); // runtime error
        }
    }
}

// Reads an entire file and feeds contents to VM to
// compiler and interpret it.
static void RunFile(std::string_view path) {
    // Turn file into a string.
    std::string filepath(path);
    std::ifstream istream(filepath);

    if (!istream.is_open()) {
        std::cerr << "File does not exist" << "\n";
        exit(74);
    }
    
    std::stringstream buffer;
    buffer << istream.rdbuf();

    std::string source = buffer.str();

    stronk::Compiler compiler;
    stronk::VirtualMachine vm;

    if (compiler.Compile(source)) {
        exit(65); // compile time error
    }

    stronk::Bytecode bytecode = compiler.GetBytecode();

    if (vm.Interpret(bytecode)) {
        exit(70); // runtime error
    }
}

auto main(int argc, const char *argv[]) -> int {
    if (argc == 1) {
        Repl();
    } else if (argc == 2) {
        RunFile(argv[1]);
    } else {
        std::cerr << "Usage: stronk [path]\n";
        exit(64);
    }
    return 0;
}

#undef COMPUTE_PERF
