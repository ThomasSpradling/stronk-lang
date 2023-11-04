#ifndef _STRONK_TOKEN_H
#define _STRONK_TOKEN_H

#include <string>
#include <memory>

enum class TokenType {
    // Single character tokens
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACKET, RIGHT_BRACKET,
    LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT,
    MINUS, PLUS,
    SEMICOLON, SLASH, STAR,
    QUOTE,

    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    DOLLAR_BRACE,

    // Literals
    IDENTIFIER, TEXT, INT, REAL,

    // Keywords
    AND, CLASS, ELSE, FALSE,
    FOR, FUN, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS,
    TRUE, VAR, WHILE,

    // Type
    PRIMITIVE,

    // Error token passed to compiler.
    ERROR,
    
    TOKEN_EOF
};

struct Token {
    TokenType type_;
    int position_;
    int line_;

    Token(TokenType type, int pos, int line) : type_(type), position_(pos), line_(line) {}
    virtual ~Token() = default;

    virtual auto ToString() const -> std::string {
        switch (type_) {
            case TokenType::LEFT_PAREN: return "LEFT_PAREN";
            case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
            case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
            case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
            case TokenType::LEFT_BRACE: return "LEFT_BRACE";
            case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
            case TokenType::COMMA: return "COMMA";
            case TokenType::DOT: return "DOT";
            case TokenType::MINUS: return "MINUS";
            case TokenType::PLUS: return "PLUS";
            case TokenType::SEMICOLON: return "SEMICOLON";
            case TokenType::SLASH: return "SLASH";
            case TokenType::STAR: return "STAR";
            case TokenType::QUOTE: return "QUOTE";
            case TokenType::BANG: return "BANG";
            case TokenType::BANG_EQUAL: return "BANG_EQUAL";
            case TokenType::EQUAL: return "EQUAL";
            case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
            case TokenType::GREATER: return "GREATER";
            case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
            case TokenType::LESS: return "LESS";
            case TokenType::LESS_EQUAL: return "LESS_EQUAL";
            case TokenType::DOLLAR_BRACE: return "DOLLAR_BRACE";
            case TokenType::IDENTIFIER: return "IDENTIFIER";
            case TokenType::TEXT: return "TEXT";
            case TokenType::INT: return "INT";
            case TokenType::REAL: return "REAL";
            case TokenType::AND: return "AND";
            case TokenType::CLASS: return "CLASS";
            case TokenType::ELSE: return "ELSE";
            case TokenType::FALSE: return "FALSE";
            case TokenType::FOR: return "FOR";
            case TokenType::FUN: return "FUN";
            case TokenType::IF: return "IF";
            case TokenType::NIL: return "NIL";
            case TokenType::OR: return "OR";
            case TokenType::PRINT: return "PRINT";
            case TokenType::RETURN: return "RETURN";
            case TokenType::SUPER: return "SUPER";
            case TokenType::THIS: return "THIS";
            case TokenType::TRUE: return "TRUE";
            case TokenType::VAR: return "VAR";
            case TokenType::WHILE: return "WHILE";
            case TokenType::PRIMITIVE: return "PRIMITIVE";
            case TokenType::ERROR: return "ERROR";
            case TokenType::TOKEN_EOF: return "TOKEN_EOF";
            default: return "UNKNOWN";
        }
    }
};

enum class PrimitiveType {
    INT,
    REAL,
    CHAR,
    BOOL
};

template <class T>
struct ValueToken : public Token {
    T value_;

    ValueToken(TokenType type, int pos, int line, T value) : Token(type, pos, line), value_(std::move(value)) {}

    auto ToString() const -> std::string override {
        std::string res = Token::ToString() + "\t\t'";
        if constexpr (std::is_same_v<T, PrimitiveType>) {
            switch(value_) {
                case PrimitiveType::INT: res += "INT"; break;
                case PrimitiveType::REAL: res += "REAL"; break;
                case PrimitiveType::CHAR: res += "CHAR"; break;
                case PrimitiveType::BOOL: res += "BOOL"; break;
                default: res += "Unknown PrimitiveType";
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            res += value_;
        } else {
            res += std::to_string(value_);
        }
        res += "'";
        return res;
    }
};

// Type token has a bit-width.
struct TypeToken : public ValueToken<PrimitiveType> {
    int width_;

    TypeToken(TokenType type, int pos, int line, PrimitiveType value, int width) : ValueToken<PrimitiveType>(type, pos, line, value), width_(width) {}
};

auto operator==(const std::shared_ptr<Token> &token1, const std::shared_ptr<Token> &token2) -> bool;

#endif // _STRONK_TOKEN_H