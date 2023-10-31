#ifndef _STRONK_PARSER_H
#define _STRONK_PARSER_H

#include "common.h"
#include "code_generator.h"
#include "token.h"

class Parser {
public:
    Parser() = default;
    void AddToken(const Token &token);
    auto Parse() -> Bytecode;
};

#endif // _STRONK_PARSER_H