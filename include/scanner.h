#ifndef _STRONK_SCANNER_H
#define _STRONK_SCANNER_H

#include <string>

enum TokenType {
    // Single character tokens
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT,
    MINUS, PLUS,
    SEMICOLON, SLASH, STAR,

    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Keywords
    AND, CLASS, ELSE, FALSE,
    FOR, FUN, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS,
    TRUE, VAR, WHILE,

    // Error token passed to compiler.
    ERROR,
    
    TOKEN_EOF
};

struct Token {
    TokenType type;
    std::string_view::iterator start;
    int length;
    int line;
};

// Lexical analysis over the source to allow for
// compiling.
class Scanner {
private:
    std::string_view _source;
    std::string_view::iterator _start;
    std::string_view::iterator _current;
    int _line;
    auto MakeToken(TokenType type) -> Token;
    auto MakeErrorToken(std::string_view message) -> Token;
public:
    Scanner(const std::string_view source);
    auto ScanToken() -> Token;
};

#endif // _STRONK_SCANNER_H