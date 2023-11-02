#include <iostream>
#include "parser.h"

// Adds a sinle token to the token list.
void Parser::AddToken(std::shared_ptr<Token> token) {
    tokens.push_back(std::move(token));
}

// Parses the token list and returns corresponding bytecode.
void Parser::Parse() {
    current = tokens.begin();
    previous = tokens.end();
    ParseTerm();
}

// Grammar: term -> factor ( (+ | -) factor )*
auto Parser::ParseTerm() -> Address {
    Address dest = ParseFactor();
    for (;;) {
        Token &tok = **current;
        Address a;
        Address b;

        switch (tok.type_) {
            case TokenType::TOKEN_EOF: return dest;
            case TokenType::PLUS:
                StepForward();
                a = dest;
                b = ParseFactor();

                dest = num_gen.GenerateTemp();

                EmitInstruction(dest, OpCode::ADD, a, b);
                break;
            case TokenType::MINUS:
                StepForward();
                a = dest;
                b = ParseFactor();

                dest = num_gen.GenerateTemp();

                EmitInstruction(dest, OpCode::SUB, a, b);
                break;
            default:
                ErrorAt(*previous, "Unexpected token.");
        }
    }
    return dest;
}

// Grammar: unary -> term ( (* | /) unary )*
auto Parser::ParseFactor() -> Address {
    Address dest = ParseUnary();
    for (;;) {
        Token &tok = **current;
        Address a;
        Address b;

        OpCode op;

        switch (tok.type_) {
            case TokenType::SLASH:
                StepForward();
                a = dest;
                b = ParseUnary();

                dest = num_gen.GenerateTemp();

                EmitInstruction(dest, OpCode::DIV, a, b);
                break;
            case TokenType::STAR:
                StepForward();
                a = dest;
                b = ParseUnary();

                dest = num_gen.GenerateTemp();

                EmitInstruction(dest, OpCode::MULT, a, b);
                break;
            default:
                return dest;
        }
    }
    return dest;
}

// Grammar: unary -> - unary | primary
auto Parser::ParseUnary() -> Address {
    TokenType op = current->get()->type_;

    if (op == TokenType::MINUS) {
        StepForward();
        Address a = ParseUnary();
        Address dest = num_gen.GenerateTemp();
        EmitInstruction(dest, OpCode::NEGATE, a);
        return dest;
    }
    return ParsePrimary();
}

// Grammar: primary -> TRUE | FALSE | NIL | INT | REAL | IDENTIFIER
auto Parser::ParsePrimary() -> Address {
    TokenType a = current->get()->type_;
    StepForward();

    Address dest;
    ValueToken<std::string> *b;

    ValueToken<float> *value_float;
    ValueToken<int> *value_int;

    switch (a) {
        case TokenType::TRUE: return EmitConstInstruction(BoolValue{true});
        case TokenType::FALSE:  return EmitConstInstruction(BoolValue{false});
        case TokenType::NIL: return EmitConstInstruction(NilValue{});
        case TokenType::REAL:
            value_float = dynamic_cast<ValueToken<float> *>(previous->get());
            if (value_float == nullptr) {
                ErrorAt(*previous, "Expected float.");
            } else {
                return EmitConstInstruction(RealValue(value_float->value_));
            }
            break;
        case TokenType::INT:
            value_int = dynamic_cast<ValueToken<int> *>(previous->get());
            if (value_int == nullptr) {
                 ErrorAt(*previous, "Expected float.");
            } else {
                return EmitConstInstruction(RealValue(value_int->value_));
            }
            break;
        case TokenType::IDENTIFIER:
            b = dynamic_cast<ValueToken<std::string> *>(previous->get());
            if (b == nullptr) {
                ErrorAt(*previous, "Unexpected token.");
            } else {
                dest = b->value_;
            }
            break;
        default:
            ErrorAt(*previous, "Unexpected token.");
    }
    return dest;
}

template <typename... Args>
void Parser::EmitInstruction(Address &dest, OpCode op, Args... args) {
    int line = previous->get()->line_;
    int position = previous->get()->position_;
    std::vector<Address> args_vec = {args...};
    cg.AddInstruction(std::make_shared<PureInstr>(op, dest, args_vec, line, position));
}

auto Parser::EmitConstInstruction(const Value &val) -> Address {
    Address dest = num_gen.GenerateTemp();
    int line = previous->get()->line_;
    int position = previous->get()->position_;
    cg.AddConstantInstruction(dest, val, line, position);
    return dest;
}

auto Parser::GetBytecode() -> Bytecode {
    cg.DissasembleCode();
    return cg.GetCode();
}

// Grabs next non-error token. Returns false if
// an error occurs and true otherwise.
void Parser::StepForward() {
    previous = current;

    for (;;) {
        current++;
        if ((*current)->type_ != TokenType::ERROR) {
            break;
        }

        auto error_token = dynamic_cast<ValueToken<std::string> *>(current->get());
        if (error_token == nullptr) {
            ErrorAt(*current, "Unexpected token.");
        } else {
            ErrorAt(*current, error_token->value_);
        }
    }
}

// Steps forward if the current token matches the passed
// in token type. If it doesn, creates an error with message
// `message`.
void Parser::StepIfMatch(TokenType type, std::string_view message) {
    if ((*current)->type_ == type) {
        StepForward();
        return;
    }

    ErrorAt(*current, message);
}

void Parser::ErrorAt(std::shared_ptr<Token> &token, std::string_view message) {
    if (is_panic_mode) {
        return;
    }
    is_panic_mode = true;
    std::cerr << "[line " << token->line_ << "] Error";

    if (token->type_ == TokenType::TOKEN_EOF) {
        std::cerr << " at end";
    } else if (token->type_ != TokenType::ERROR) {
        std::cerr << " at '";
        token->ToString();
        std::cerr << "'";
    }

    std::cerr << ": " << message << "\n";
    error_occurred = true;
}