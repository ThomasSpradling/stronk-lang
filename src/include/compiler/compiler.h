#ifndef _STRONK_COMPILER_H
#define _STRONK_COMPILER_H

#include <string>
#include "common/common.h"
#include "frontend/scanner.h"
#include "frontend/parser.h"

namespace stronk {

// Compiles tokens provided by Scanner into
// bytecode.
class Compiler {
private:
    Scanner scanner_;
    Parser parser_;
    Bytecode bytecode_;
public:
    Compiler() = default;
    auto Compile(std::string_view source) -> bool;
    auto GetBytecode() -> Bytecode;
};

} // namespace "stronk"

#endif // _STRONK_COMPILER_H