#include "scanner.h"

Scanner::Scanner(const std::string_view source) : _start(source.begin()), _current(source.begin()), _line(1), _source(source) {}

// Utility mathod to determine whether the current character
// matches the passed in character.
auto Scanner::MatchChar(char to_match) -> bool {
    if (_current == _source.end() || *_current != to_match) {
        return false;
    }
    _current++;
    return true;
}

// Utility method to skip all whitespace and comments at
// current character.
void Scanner::SkipWhitespace() {
    for (;;) {
        switch (*_current) {
            case ' ':
            case '\r':
            case '\t':
                _current++;
                break;
            case '/':
                if (*(_current + 1) == '/') {
                    while (*_current != '\n' && _current != _source.end()) {
                        _current++;
                    }
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

// Scans a single token from the input source.
auto Scanner::ScanToken() -> Token {
    SkipWhitespace();
    _start = _current;

    if (_current == _source.end()) {
        return { TokenType::TOKEN_EOF };
    }

    // Get next character and move forward.
    char c = *(_current++);

    if (IsDigit(c)) {
        return ScanNumber();
    }

    switch (c) {
        case '(': return MakeToken(TokenType::LEFT_PAREN);
        case ')': return MakeToken(TokenType::RIGHT_PAREN);
        case '{': return MakeToken(TokenType::LEFT_BRACE);
        case '}': return MakeToken(TokenType::RIGHT_BRACE);
        case ';': return MakeToken(TokenType::SEMICOLON);
        case ',': return MakeToken(TokenType::COMMA);
        case '.': return MakeToken(TokenType::DOT);
        case '-': return MakeToken(TokenType::MINUS);
        case '+': return MakeToken(TokenType::PLUS);
        case '/': return MakeToken(TokenType::SLASH);
        case '*': return MakeToken(TokenType::STAR);
        case '!': return MakeToken(
            MatchChar('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        case '=': return MakeToken(
            MatchChar('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        case '<': return MakeToken(
            MatchChar('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        case '>': return MakeToken(
            MatchChar('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        case '"': return ScanString();
    }

    return MakeErrorToken("Unexpected character.");
}

// Scans through a string.
auto Scanner::ScanString() -> Token {
    while (*_current != '"' && _current != _source.end()) {
        if (*_current == '\n') {
            // Account for newline characters.
            _line++;
        }
        _current++;
    }

    if (_current == _source.end()) {
        return MakeErrorToken("Unterminated string.");
    }

    _current++;
    return MakeToken(TokenType::STRING);
}

// Utility method to determine where a character is
// a digit.
auto Scanner::IsDigit(char c) -> bool {
    return c >= '0' && c <= '9';
}


// Scans through numbers.
auto Scanner::ScanNumber() -> Token {
    while (IsDigit(*_current)) {
        _current++;
    }

    if (*_current == '.' && IsDigit(*(_current + 1))) {
        _current++;

        while (IsDigit(*_current)) {
            _current++;
        }
    }

    return MakeToken(TokenType::NUMBER);
}

// Helper method to build a token with `type`.
auto Scanner::MakeToken(TokenType type) -> Token {
    return { type, _start, static_cast<int>(_current - _start), _line };
}

// Helper method to build a token with error message.
auto Scanner::MakeErrorToken(std::string_view message) -> Token {
    return { TokenType::ERROR, message.begin(), static_cast<int>(message.length()), _line };
}