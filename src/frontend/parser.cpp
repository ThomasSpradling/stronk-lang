#include <iostream>
#include "frontend/parser.h"

// Adds a sinle token to the token list.
void Parser::AddToken(std::shared_ptr<Token> token) {
    tokens_.push_back(std::move(token));
}

// Parses the token list and returns corresponding bytecode.
void Parser::Parse() {
    current_ = tokens_.begin();
    previous_ = tokens_.end();
    ParseTerm();
    if (current_->get()->type_ != TokenType::TOKEN_EOF) {
        ErrorAt(*current_, "Expected end of file.");
    }
}

// Grammar: term -> factor ( (+ | -) factor )*
auto Parser::ParseTerm() -> Address {
    Address dest = ParseFactor();
    for (;;) {
        Token &tok = **current_;
        Address a;
        Address b;

        switch (tok.type_) {
            case TokenType::TOKEN_EOF: return dest;
            case TokenType::PLUS:
                StepForward();
                a = dest;
                b = ParseFactor();

                dest = num_gen_.GenerateTemp();

                EmitInstruction(dest, OpCode::ADD, a, b);
                break;
            case TokenType::MINUS:
                StepForward();
                a = dest;
                b = ParseFactor();

                dest = num_gen_.GenerateTemp();

                EmitInstruction(dest, OpCode::SUB, a, b);
                break;
            default:
                ErrorAt(*previous_, "Unexpected token.");
        }
    }
    return dest;
}

// Grammar: unary -> term ( (* | /) unary )*
auto Parser::ParseFactor() -> Address {
    Address dest = ParseUnary();
    for (;;) {
        Token &tok = **current_;
        Address a;
        Address b;

        switch (tok.type_) {
            case TokenType::TOKEN_EOF: return dest;
            case TokenType::SLASH:
                StepForward();
                a = dest;
                b = ParseUnary();

                dest = num_gen_.GenerateTemp();

                EmitInstruction(dest, OpCode::DIV, a, b);
                break;
            case TokenType::STAR:
                StepForward();
                a = dest;
                b = ParseUnary();

                dest = num_gen_.GenerateTemp();

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
    TokenType op = current_->get()->type_;

    if (op == TokenType::MINUS) {
        StepForward();
        Address a = ParseUnary();
        Address dest = num_gen_.GenerateTemp();
        EmitInstruction(dest, OpCode::NEGATE, a);
        return dest;
    }
    return ParsePrimary();
}

// Grammar: primary -> TRUE | FALSE | NIL | INT | REAL | IDENTIFIER
auto Parser::ParsePrimary() -> Address {
    TokenType a = current_->get()->type_;

    Address dest;
    ValueToken<std::string> *b;

    ValueToken<float> *value_float;
    ValueToken<int> *value_int;

    switch (a) {
        case TokenType::TOKEN_EOF: return dest;
        case TokenType::TRUE: return EmitConstInstruction(BoolValue{true});
        case TokenType::FALSE:  return EmitConstInstruction(BoolValue{false});
        case TokenType::NIL: return EmitConstInstruction(NilValue{});
        case TokenType::REAL:
            StepForward();
            value_float = dynamic_cast<ValueToken<float> *>(previous_->get());
            if (value_float == nullptr) {
                ErrorAt(*previous_, "Expected float.");
            } else {
                return EmitConstInstruction(RealValue(value_float->value_));
            }
            break;
        case TokenType::INT:
            StepForward();
            value_int = dynamic_cast<ValueToken<int> *>(previous_->get());
            if (value_int == nullptr) {
                 ErrorAt(*previous_, "Expected float.");
            } else {
                return EmitConstInstruction(RealValue(value_int->value_));
            }
            break;
        case TokenType::IDENTIFIER:
            StepForward();
            b = dynamic_cast<ValueToken<std::string> *>(previous_->get());
            if (b == nullptr) {
                ErrorAt(*previous_, "Unexpected token.");
            } else {
                dest = b->value_;
            }
            break;
        default:
            ErrorAt(*previous_, "Unexpected token.");
    }
    return dest;
}

template <typename... Args>
void Parser::EmitInstruction(Address &dest, OpCode op, Args... args) {
    int line = previous_->get()->line_;
    int position = previous_->get()->position_;
    std::vector<Address> args_vec = {args...};
    cg_.AddInstruction(std::make_shared<PureInstr>(op, dest, args_vec, line, position));
}

auto Parser::EmitConstInstruction(const Value &val) -> Address {
    Address dest = num_gen_.GenerateTemp();
    int line = previous_->get()->line_;
    int position = previous_->get()->position_;
    cg_.AddConstantInstruction(dest, val, line, position);
    return dest;
}

auto Parser::GetBytecode() -> Bytecode {
    cg_.DissasembleCode();
    return cg_.GetCode();
}

// Grabs next non-error token. Returns false if
// an error occurs and true otherwise.
void Parser::StepForward() {
    previous_ = current_;

    for (;;) {
        current_++;
        if ((*current_)->type_ != TokenType::ERROR) {
            break;
        }

        auto error_token = dynamic_cast<ValueToken<std::string> *>(current_->get());
        if (error_token == nullptr) {
            ErrorAt(*current_, "Unexpected token.");
        } else {
            ErrorAt(*current_, error_token->value_);
        }
    }
}

// Steps forward if the current token matches the passed
// in token type. If it doesn, creates an error with message
// `message`.
void Parser::StepIfMatch(TokenType type, std::string_view message) {
    if ((*current_)->type_ == type) {
        StepForward();
        return;
    }

    ErrorAt(*current_, message);
}

void Parser::ErrorAt(std::shared_ptr<Token> &token, std::string_view message) {
    if (is_panic_mode_) {
        return;
    }
    is_panic_mode_ = true;
    std::cerr << "[line " << token->line_ << "] Error";

    if (token->type_ == TokenType::TOKEN_EOF) {
        std::cerr << " at end";
    } else if (token->type_ != TokenType::ERROR) {
        std::cerr << " at '";
        token->ToString();
        std::cerr << "'";
    }

    std::cerr << ": " << message << "\n";
    error_occurred_ = true;
}