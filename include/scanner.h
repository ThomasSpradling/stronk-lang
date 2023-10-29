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
    QUOTE,

    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    DOLLAR_BRACE, ESCAPED,

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

enum ScannerMode {
    MODE_NORMAL,
    MODE_STRING
};

// Lexical analysis over the source to allow for
// compiling.
class Scanner {
private:
    std::string_view _source;
    std::string_view::iterator _start;
    std::string_view::iterator _current;
    ScannerMode mode = ScannerMode::MODE_NORMAL;
    int str_depth = 0;
    int _line = 1;
    
    auto MakeToken(TokenType type) -> Token;
    auto MatchChar(char to_match) -> bool;
    void SkipWhitespace();
    auto MakeErrorToken(std::string_view message) -> Token;

    auto ScanString() -> Token;

    auto IsDigit(char c) -> bool;
    auto ScanNumber() -> Token;

    auto IsAlpha(char c) -> bool;
    auto CheckKeyword(int start, int length, std::string_view rest, TokenType type) -> Token;
    auto ScanIdentifier() -> Token;
public:
    Scanner(const std::string_view source);
    auto ScanToken() -> Token;
};

#endif // _STRONK_SCANNER_H