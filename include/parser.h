#ifndef _STRONK_PARSER_H
#define _STRONK_PARSER_H

#include "common.h"
#include "code_generator.h"
#include "token.h"

class Parser {
private:
    CodeGenerator cg;

    std::vector<std::unique_ptr<Token>> tokens;
    std::vector<std::unique_ptr<Token>>::iterator current;
    std::vector<std::unique_ptr<Token>>::iterator previous;
    bool error_occurred = false;
    bool is_panic_mode = false; // Prevents cascade of errors.

    void StepForward();
    void StepIfMatch(TokenType type, std::string_view message);
    void ErrorAt(std::unique_ptr<Token> &token, std::string_view message);

    auto ParseExpression() -> Address;
    auto ParseFactor() -> Address;
    auto ParseUnary() -> Address;
    auto ParsePrimary() -> Address;


public:
    Parser() : current(tokens.begin()), previous(tokens.begin()) {}
    void AddToken(std::unique_ptr<Token> token);
    void Parse();
    auto GetBytecode() -> Bytecode;
};

#endif // _STRONK_PARSER_H