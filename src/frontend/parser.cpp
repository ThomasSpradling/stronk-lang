#include <iostream>
#include "frontend/parser.h"

#define PARSE_CASE(func, opcode) \
    StepForward(); \
    a = dest; \
    b = func(); \
    \
    dest = num_gen_.GenerateTemp(); \
    \
    EmitInstruction(dest, opcode, a, b); \
    break;

// Adds a sinle token to the token list.
void Parser::AddToken(std::shared_ptr<Token> token) {
    tokens_.push_back(std::move(token));
}

// Parses the token list and returns corresponding bytecode.
void Parser::Parse() {
    current_ = tokens_.begin();
    previous_ = tokens_.end();
    if (current_->get()->type_ == TokenType::TOKEN_EOF) {
        return;
    }
    ParseExpression();
    if (current_->get()->type_ != TokenType::TOKEN_EOF) {
        ErrorAt(*current_, "Expected end of file.");
    }
}

auto Parser::ParseExpression() -> Address {
    return ParseLogicOr();
}

// Grammar: logic_or -> logic_and ( "or" logic_and )*
auto Parser::ParseLogicOr() -> Address {
    Address dest = ParseLogicAnd();
    for(;;) {
        Token &tok = **current_;
        Address a;
        Address b;

        switch (tok.type_) {
            case TokenType::OR: PARSE_CASE(ParseLogicAnd, OpCode::OR)
            default: return dest;
        }
    }
}

// Grammar: logic_and -> equality ( "and" equality )*
auto Parser::ParseLogicAnd() -> Address {
    Address dest = ParseEquality();
    for(;;) {
        Token &tok = **current_;
        Address a;
        Address b;

        switch (tok.type_) {
            case TokenType::AND: PARSE_CASE(ParseEquality, OpCode::AND)
                break;
            default: return dest;
        }
    }
}

// Grammar: equality -> comparison ( ( "!=" | "==" ) comparison )*
auto Parser::ParseEquality() -> Address {
    Address dest = ParseComparison();
    for(;;) {
        Token &tok = **current_;
        Address a;
        Address b;

        switch (tok.type_) {
            case TokenType::EQUAL_EQUAL: PARSE_CASE(ParseComparison, OpCode::EQ)
            case TokenType::BANG_EQUAL: PARSE_CASE(ParseComparison, OpCode::NEQ)
            default: return dest;
        }
    }
}

// Grammar: comparison -> term ( (">" | ">=" | "<" | "<=" ) term )*
auto Parser::ParseComparison() -> Address {
    Address dest = ParseTerm();
    for(;;) {
        Token &tok = **current_;
        Address a;
        Address b;

        switch (tok.type_) {
            case TokenType::GREATER: PARSE_CASE(ParseTerm, OpCode::GT)
            case TokenType::GREATER_EQUAL: PARSE_CASE(ParseTerm, OpCode::GEQ)
            case TokenType::LESS: PARSE_CASE(ParseTerm, OpCode::LT)
            case TokenType::LESS_EQUAL: PARSE_CASE(ParseTerm, OpCode::LEQ)
            default: return dest;
        }
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
            case TokenType::PLUS: PARSE_CASE(ParseFactor, OpCode::ADD)
            case TokenType::MINUS: PARSE_CASE(ParseFactor, OpCode::SUB)
            default: return dest;
        }
    }
    return dest;
}

// Grammar: factor -> unary ( (* | /) unary )*
auto Parser::ParseFactor() -> Address {
    Address dest = ParseUnary();
    for (;;) {
        Token &tok = **current_;
        Address a;
        Address b;

        switch (tok.type_) {
            case TokenType::SLASH: PARSE_CASE(ParseUnary, OpCode::DIV)
            case TokenType::STAR: PARSE_CASE(ParseUnary, OpCode::MULT)
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

// Grammar:
//      primary -> TRUE | FALSE | NIL | INT | REAL | IDENTIFIER
//              | "(" expression ")"
auto Parser::ParsePrimary() -> Address {
    TokenType a = current_->get()->type_;

    Address dest;
    ValueToken<std::string> *b;

    ValueToken<float> *value_float;
    ValueToken<int> *value_int;

    switch (a) {
        case TokenType::TRUE:
            StepForward();
            return EmitConstInstruction(true);
        case TokenType::FALSE:
            StepForward();
            return EmitConstInstruction(false);
        case TokenType::NIL:
            StepForward();
            return EmitConstInstruction(nullptr);
        case TokenType::REAL:
            StepForward();
            value_float = dynamic_cast<ValueToken<float> *>(previous_->get());
            if (value_float == nullptr) {
                ErrorAt(*previous_, "Expected float.");
            } else {
                return EmitConstInstruction(value_float->value_);
            }
            break;
        case TokenType::INT:
            StepForward();
            value_int = dynamic_cast<ValueToken<int> *>(previous_->get());
            if (value_int == nullptr) {
                 ErrorAt(*previous_, "Expected integer.");
            } else {
                return EmitConstInstruction(value_int->value_);
            }
            break;
        case TokenType::QUOTE:
            StepForward();
            dest = ParseString();
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
        case TokenType::LEFT_PAREN:
            StepForward();
            dest = ParseExpression();
            if (current_->get()->type_ != TokenType::RIGHT_PAREN) {
                ErrorAt(*previous_, "Expected end of parentheses.");
            }
            StepForward();
            break;
        default:
            return dest;
    }
    return dest;
}

// Grammar:
// string -> ( TEXT | "${" expression "}" )* QUOTE
auto Parser::ParseString() -> Address {
    Address dest;
    if (current_->get()->type_ == TokenType::QUOTE) {
        return EmitConstInstruction("");
    }
    for(;;) {
        Token &tok = **current_;
        Address a;
        Address b;

        ValueToken<std::string> *value;

        switch (tok.type_) {
            case TokenType::TEXT: // ti = const val, ti+1 = concat dest ti
                StepForward();
                value = dynamic_cast<ValueToken<std::string> *>(previous_->get());
                if (value == nullptr) {
                    ErrorAt(*previous_, "Expected string.");
                } else if (dest.empty()) {
                    dest = EmitConstInstruction(value->value_);
                } else {
                    a = dest;
                    b = EmitConstInstruction(value->value_);

                    dest = num_gen_.GenerateTemp();
                    EmitInstruction(dest, OpCode::CONCAT, a, b);
                }
                break;
            case TokenType::DOLLAR_BRACE:
                StepForward();
                if (dest.empty()) {
                    a = ParseExpression();
                    dest = num_gen_.GenerateTemp();
                    EmitInstruction(dest, OpCode::TO_STRING, a);
                } else {
                    a = dest;
                    b = ParseExpression();
                    dest = num_gen_.GenerateTemp();
                    EmitInstruction(dest, OpCode::TO_STRING, a);

                    dest = num_gen_.GenerateTemp();
                    EmitInstruction(dest, OpCode::CONCAT, a, b);
                }
                if (current_->get()->type_ != TokenType::RIGHT_BRACE) {
                    ErrorAt(*current_, "Expected right brace '}' after interpolation.");
                }
                StepForward();
                break;
            case TokenType::QUOTE:
                StepForward();
            default: return dest;
        }
    }
}

template <typename... Args>
void Parser::EmitInstruction(Address &dest, OpCode op, Args... args) {
    int line = previous_->get()->line_;
    int position = previous_->get()->position_;
    std::vector<Address> args_vec = {args...};
    cg_.AddInstruction(std::make_shared<PureInstr>(op, dest, args_vec, line, position));
}

auto Parser::EmitConstInstruction(const ConstantPool::ConstantValue &val) -> Address {
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