#include <iomanip>
#include <iostream>
#include <string>
#include "compiler.h"

// Compiles the source after scanning it.
auto Compiler::Compile(std::string_view source) -> bool {
    scanner.LoadSource(source);
    std::vector<Token> tokens;
    
    #if DEBUG_TRACE_EXECUTION
    int line = -1;
    #endif

    for (;;) {
        std::unique_ptr<Token> token = scanner.ScanNextToken();
        auto token_line = token->line_;
        auto token_type = token->type_;
        std::string token_form = token->ToString();
        parser.AddToken(std::move(token));

        #if DEBUG_TRACE_EXECUTION
        if (token_line != line) {
            std::cout << std::setw(4) << std::setfill(' ') << " " + std::to_string(token_line);
            line = token_line;
        } else {
            std::cout << "   |";
        }

        std::cout << " " << token_form << "\n";
        #endif

        if (token_type == TokenType::TOKEN_EOF) {
            break;
        }
    }

    parser.Parse();

    bytecode = parser.GetBytecode();
    return true;
}

auto Compiler::GetBytecode() -> Bytecode {
    return bytecode;
}