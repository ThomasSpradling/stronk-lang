#ifndef _STRONK_COMPILER_H
#define _STRONK_COMPILER_H

#include <string>

// Compiles tokens provided by Scanner into
// bytecode.
class Compiler {
public:
    Compiler();
    void compile(const std::string source);
};

#endif // _STRONK_COMPILER_H