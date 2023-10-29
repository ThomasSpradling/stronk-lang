#include <iomanip>
#include <iostream>
#include "compiler.h"

Compiler::Compiler(const std::string_view source) : _scanner(source) {};

// Grabs next non-error token. Returns false if
// an error occurs and true otherwise.
void Compiler::StepForward() {
    previous = current;

    for (;;) {
        current = _scanner.ScanToken();
        if (current.type != TokenType::ERROR) {
            break;
        }

        ErrorAt(current, current.start);
    }
}

// Steps forward if the current token matches the passed
// in token type. If it doesn, creates an error with message
// `message`.
void Compiler::StepIfMatch(TokenType type, std::string_view message) {
    if (current.type == type) {
        StepForward();
        return;
    }

    ErrorAt(current, message);
}

void Compiler::ErrorAt(Token &token, std::string_view message) {
    if (is_panic_mode) {
        return;
    }
    is_panic_mode = true;
    std::cerr << "[line " << token.line << "] Error";

    if (token.type == TokenType::TOKEN_EOF) {
        std::cerr << " at end";
    } else if (token.type != TokenType::ERROR) {
        std::cerr << " at '";
        std::cerr.write(token.start, token.length);
        std::cerr << "'";
    }

    std::cerr << ": " << message << "\n";
    error_occurred = true;
}

// Compiles the source after scanning it.
auto Compiler::Compile(Chunk &chunk) -> bool {
    current_chunk = chunk;

    StepForward();
    StepIfMatch(TokenType::TOKEN_EOF, "Expect end of expression.");

    // End with return.
    current_chunk.AddChunk(OpCode::OP_RETURN, previous.line);
    return !error_occurred;
}