#include <vector>
#include "frontend/token.h"
#include "frontend/compiler.h"

/* Utility functions used in source and primarily for testing. */

auto ReadTokensFromSource(const std::string &source) -> std::vector<std::shared_ptr<Token>>;
auto ReadBytecodeFromTokens(const std::vector<std::shared_ptr<Token>> &tokens) -> Bytecode;