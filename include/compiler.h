#ifndef _STRONK_COMPILER_H
#define _STRONK_COMPILER_H

#include <string>
#include "chunk.h"
#include "common.h"
#include "scanner.h"
#include "parser.h"

// Compiles tokens provided by Scanner into
// bytecode.
class Compiler {
private:
    Scanner scanner;
    Parser parser;
    Bytecode bytecode;
public:
    Compiler() = default;
    auto Compile(std::string_view source) -> bool;
    auto GetBytecode() -> Bytecode;
};

#endif // _STRONK_COMPILER_H