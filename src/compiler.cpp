#include "compiler.h"
#include "common.h"
#include "scanner.h"

Compiler::Compiler() = default;

// Compiles the source after scanning it.
void Compiler::Compile(const std::string_view source) {
    Scanner scanner(source);
}