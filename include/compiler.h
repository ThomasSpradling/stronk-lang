#ifndef _STRONK_COMPILER_H
#define _STRONK_COMPILER_H

#include <string>
#include "chunk.h"
#include "common.h"
#include "scanner.h"

// Compiles tokens provided by Scanner into
// bytecode.
class Compiler {
private:
    Scanner _scanner;
    Token current;
    Token previous;
    bool error_occurred = false;
    bool is_panic_mode = false; // Prevents cascade errors.
    Chunk &current_chunk;

    void StepForward();
    void StepIfMatch(TokenType type, std::string_view message);
    void ErrorAt(Token &token, std::string_view message);
    void ParseNumber();
public:
    Compiler(const std::string_view source, Chunk &chunk);
    
    auto Compile() -> bool;
};

#endif // _STRONK_COMPILER_H