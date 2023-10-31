#include <iomanip>
#include <iostream>
#include <string>
#include "compiler.h"



// Grabs next non-error token. Returns false if
// an error occurs and true otherwise.
// void Compiler::StepForward() {
//     previous = current;

//     for (;;) {
//         current = _scanner.ScanToken();
//         if (current.type != TokenType::ERROR) {
//             break;
//         }

//         ErrorAt(current, current.start);
//     }
// }

// Steps forward if the current token matches the passed
// in token type. If it doesn, creates an error with message
// `message`.
// void Compiler::StepIfMatch(TokenType type, std::string_view message) {
//     if (current.type == type) {
//         StepForward();
//         return;
//     }

//     ErrorAt(current, message);
// }

// void Compiler::ErrorAt(Token &token, std::string_view message) {
//     if (is_panic_mode) {
//         return;
//     }
//     is_panic_mode = true;
//     std::cerr << "[line " << token.line << "] Error";

//     if (token.type == TokenType::TOKEN_EOF) {
//         std::cerr << " at end";
//     } else if (token.type != TokenType::ERROR) {
//         std::cerr << " at '";
//         std::cerr.write(token.start, token.length);
//         std::cerr << "'";
//     }

//     std::cerr << ": " << message << "\n";
//     error_occurred = true;
// }

// Compiles the source after scanning it.
auto Compiler::Compile(std::string_view source) -> bool {
    // End with return.
    // StepForward();
    // StepIfMatch(TokenType::TOKEN_EOF, "Expected end of expression.");
    // current_chunk.AddChunk(OpCode::OP_RETURN, previous.line);
    // return !error_occurred;

    scanner.LoadSource(source);
    std::vector<Token> tokens;
    
    #if DEBUG_TRACE_EXECUTION
    int line = -1;
    #endif

    for (;;) {
        std::unique_ptr<Token> token = scanner.ScanNextToken();
        // parser.AddToken();

        #if DEBUG_TRACE_EXECUTION
        if (token->line_ != line) {
            std::cout << std::setw(4) << std::setfill(' ') << " " + std::to_string(token->line_);
            line = token->line_;
        } else {
            std::cout << "   |";
        }

        std::cout << " " << token->ToString() << "\n";
        #endif

        if (token->type_ == TokenType::TOKEN_EOF) {
            break;
        }
    }

    bytecode = parser.Parse();
    return true;
}

auto Compiler::GetBytecode() -> Bytecode {
    return bytecode;
}