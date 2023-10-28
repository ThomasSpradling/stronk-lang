#include <iomanip>
#include <iostream>
#include "compiler.h"
#include "common.h"
#include "scanner.h"

Compiler::Compiler() = default;

// Compiles the source after scanning it.
void Compiler::Compile(const std::string_view source) {
    Scanner scanner(source);
    int line = -1;
    for (;;) {
        Token token = scanner.ScanToken();
        if (token.line != line) {
            std::cout << std::setw(4) << std::setfill('0') << token.line << ' ';
            line = token.line;
        } else {
            std::cout << "   | ";
        }
        std::cout << std::setw(2) << token.type << " '";
        std::cout.write(token.start, token.length);

        if (token.type == TOKEN_EOF) {
            break;
        }
    }
}