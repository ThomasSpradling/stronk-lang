#include <iostream>
#include "parser.h"

// Adds a sinle token to the token list.
void Parser::AddToken(std::unique_ptr<Token> token) {
    tokens.push_back(std::move(token));
}

// Parses the token list and returns corresponding bytecode.
void Parser::Parse() {
    ParseExpression();
}

auto Parser::ParseExpression() -> Address {
    Address dest = ParseFactor();
    for (;;) {
        TokenType op = current->get()->type_;
        Address a;
        Address b;

        StepForward();

        switch (op) {
            case TokenType::TOKEN_EOF: return dest;
            case TokenType::PLUS:
            case TokenType::MINUS:
                a = dest;
                b = ParseFactor();

                dest = GenerateTemp();

                EmitInstruction(dest, op, a, b);
                break;
            default:
                ErrorAt(*previous, "Unexpected token.");
        }
    }
    return dest;
}

auto Parser::ParseFactor() -> Address {
    Address dest = ParseUnary();
    for (;;) {
        TokenType op = current->get()->type_;
        Address a;
        Address b;

        StepForward();

        switch (op) {
            case TokenType::SLASH:
            case TokenType::STAR:
                a = dest;
                b = ParseUnary();

                dest = GenerateTemp();

                EmitInstruction(dest, op, a, b);
                break;
            default:
                ErrorAt(*previous, "Unexpected token.");
        }
    }
    return dest;
}

auto Parser::ParseUnary() -> Address {
    TokenType op = current->get()->type_;
    StepForward();

    if (op == TokenType::MINUS) {
        Address a = ParseUnary();
        Address dest = GenerateTemp();
        EmitInstruction(dest, op, a);
    } else {
        return ParsePrimary();
    }
}

auto Parser::ParsePrimary() -> Address {
    TokenType a = current->get()->type_;
    StepForward();

    Address dest;
    ValueToken<std::string> *b;

    switch (a) {
        case TokenType::TRUE:
        case TokenType::FALSE:
        case TokenType::NIL:
        case TokenType::REAL:
        case TokenType::INT:
            dest = GenerateTemp();
            EmitConstInstruction(dest, a);
            break;
        case TokenType::IDENTIFIER:
            b = dynamic_cast<ValueToken<std::string> *>(previous->get());
            if (b == nullptr) {
                ErrorAt(*previous, "Unexpected token.");
            } else {
                dest = b->value_;
            }
        default:
            ErrorAt(*previous, "Unexpected token.");
    }
    return dest;
}

auto Parser::GetBytecode() -> Bytecode {
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

void Parser::ErrorAt(std::unique_ptr<Token> &token, std::string_view message) {
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