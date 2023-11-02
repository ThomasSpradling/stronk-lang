#ifndef _STRONK_PARSER_H
#define _STRONK_PARSER_H

#include "common.h"
#include "code_generator.h"
#include "token.h"
#include "number_generator.h"

class Parser {
private:
    CodeGenerator cg;

    NumberGenerator num_gen;

    std::vector<std::shared_ptr<Token>> tokens;
    std::vector<std::shared_ptr<Token>>::iterator current;
    std::vector<std::shared_ptr<Token>>::iterator previous;
    bool error_occurred = false;
    bool is_panic_mode = false; // Prevents cascade of errors.

    void StepForward();
    void StepIfMatch(TokenType type, std::string_view message);
    void ErrorAt(std::shared_ptr<Token> &token, std::string_view message);

    auto ParseTerm() -> Address;
    auto ParseFactor() -> Address;
    auto ParseUnary() -> Address;
    auto ParsePrimary() -> Address;

    template <typename... Args> void EmitInstruction(Address &dest, OpCode op, Args... args);
    auto EmitConstInstruction(const Value &val) -> Address;
public:
    Parser() {}
    void AddToken(std::shared_ptr<Token> token);
    void Parse();
    auto GetBytecode() -> Bytecode;
};

#endif // _STRONK_PARSER_H