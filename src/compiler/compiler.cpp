#include <iomanip>
#include <iostream>
#include <string>
#include "compiler/compiler.h"

// Compiles the source after scanning it.
auto Compiler::Compile(std::string_view source) -> bool {
    scanner_.LoadSource(source);
    std::vector<Token> tokens;
    
    #ifdef DEBUG_TRACE_EXECUTION
    int line = -1;
    #endif

    for (;;) {
        std::shared_ptr<Token> token = scanner_.ScanNextToken();

        #if DEBUG_TRACE_EXECUTION
        auto token_line = token->line_;
        #endif
        
        auto token_type = token->type_;
        std::string token_form = token->ToString();
        parser_.AddToken(std::move(token));

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

    parser_.Parse();

    bytecode_ = parser_.GetBytecode();
    
    return true;
}

auto Compiler::GetBytecode() -> Bytecode {
    return bytecode_;
}