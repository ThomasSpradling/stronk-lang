#ifndef _STRONK_PARSER_H
#define _STRONK_PARSER_H

#include <vector>
#include "common/common.h"
#include "frontend/code_generator.h"
#include "frontend/token.h"
#include "common/number_generator.h"

class Parser {
private:
    CodeGenerator cg_;

    NumberGenerator num_gen_;
    NumberGenerator control_flow_gen_;

    std::vector<std::shared_ptr<Token>> tokens_;
    std::vector<std::shared_ptr<Token>>::iterator current_;
    std::vector<std::shared_ptr<Token>>::iterator previous_;
    bool error_occurred_ = false;
    bool is_panic_mode_ = false; // Prevents cascade of errors.

    void StepForward();
    void StepIfMatch(TokenType type, std::string_view message);
    void ErrorAt(std::shared_ptr<Token> &token, std::string_view message);

    void ParseDeclaration();
    void ParseVarDeclaration();
    void ParseStatement();
    void ParseExprStatement();
    void ParseForStatement();
    void ParseIfStatement();
    void ParseWhileStatement();
    void ParsePrintStatement();
    void ParseBlock();
    auto ParseExpression() -> Address;
    auto ParseAssignment() -> Address;
    auto ParseLogicOr() -> Address;
    auto ParseLogicAnd() -> Address;
    auto ParseEquality() -> Address;
    auto ParseComparison() -> Address;
    auto ParseTerm() -> Address;
    auto ParseFactor() -> Address;
    auto ParseUnary() -> Address;
    auto ParsePrimary() -> Address;
    auto ParseString() -> Address;

    template <typename... Args> void EmitInstruction(Address &dest, OpCode op, Args... args);
    auto EmitConstInstruction(const ConstantPool::ConstantValue &val) -> Address;
    auto EmitConstInstruction(Address &dest, const ConstantPool::ConstantValue &val) -> Address;
    template <typename... Args> void EmitInstruction(OpCode op, Args... args);
    void EmitBr(Address cond, Label label1, Label label2);
    void EmitLabel(Label label);
    void EmitJmp(Label label);
public:
    Parser() = default;
    void AddToken(std::shared_ptr<Token> token);
    void Parse();
    auto GetBytecode() -> Bytecode;
};

#endif // _STRONK_PARSER_H