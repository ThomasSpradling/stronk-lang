#include <iostream>
#include "scanner.h"

Scanner::Scanner(const std::string_view source) : _start(source.begin()), _current(source.begin()), _source(source) {}

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
            case '\n':
                _line++;
                _current++;
                break;
            case '/':
                if (*(_current + 1) == '/') {
                    // Comment goes until end of the line.
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
    // String mode should leave whitespace alone.
    if (mode != ScannerMode::MODE_STRING) {
        SkipWhitespace();
    }
    _start = _current;

    if (_current == _source.end()) {
        if (str_depth != 0) {
            str_depth = 0;
            return MakeErrorToken("Unterminated string.");
        }
        return MakeToken(TokenType::TOKEN_EOF);
    }

    // String mode activates while scanning within
    // a string.
    if (mode == ScannerMode::MODE_STRING) {
        return ScanString();
    }

    // Get next character and move forward.
    char c = *_current++;

    if (IsAlpha(c)) {
        return ScanIdentifier();
    }
    if (IsDigit(c)) {
        return ScanNumber();
    }

    switch (c) {
        case '(': return MakeToken(TokenType::LEFT_PAREN);
        case ')': return MakeToken(TokenType::RIGHT_PAREN);
        case '{': return MakeToken(TokenType::LEFT_BRACE);
        case '}':
            if (str_depth > 0) {
                mode = ScannerMode::MODE_STRING;
            }
            return MakeToken(TokenType::RIGHT_BRACE);
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
        case '"':
            str_depth++;
            mode = ScannerMode::MODE_STRING;
            return MakeToken(TokenType::QUOTE);
    }

    return MakeErrorToken("Unexpected character.");
}

// Assumes in string mode. Gets next token found in string.
auto Scanner::ScanString() -> Token {
    for (;;) {
        if (_current == _source.end()) {
            return MakeErrorToken("Unterminated string.");
        }

        char c = *_current++;

        switch (c) {
            case '\\':
                // Escaped characters are their own tokens.
                _current++;
                return MakeToken(TokenType::ESCAPED);
            case '"':
                mode = ScannerMode::MODE_NORMAL;
                str_depth--;
                return MakeToken(TokenType::QUOTE);
            case '$':
                if (MatchChar('{')) {
                    mode = ScannerMode::MODE_NORMAL;
                    return MakeToken(TokenType::DOLLAR_BRACE);
                }
            case '\n':
                _line++;
                break;
            default:
                // If the next characters us to cut string.
                if (_current == _source.end() || *_current == '"' || *_current  == '\\') {
                    return MakeToken(TokenType::STRING);
                }
                if (_current + 1 == _source.end() || *_current == '$' && *(_current + 1) == '{') {
                    return MakeToken(TokenType::STRING);
                }
        }
    }
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

// Determines if character a letter or underscore.
auto Scanner::IsAlpha(char c) -> bool {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; 
}

auto Scanner::CheckKeyword(int start, int length, std::string_view rest, TokenType type) -> Token {
    std::string_view::iterator start_iter = _start + start;
    std::string_view view(start_iter, length);
    if (_current - _start == start + length && view == rest) {
        return MakeToken(type);
    }

    return MakeToken(TokenType::IDENTIFIER);
}

// Scans identifier, which begin with letter (or underscore) and
// with all other characters being letters, underscores, or numbers.
auto Scanner::ScanIdentifier() -> Token {
    while (IsAlpha(*_current) || IsDigit(*_current)) {
        _current++;
    }
    switch (*_start) {
        case 'a': return CheckKeyword(1, 2, "nd", TokenType::AND);
        case 'c': return CheckKeyword(1, 4, "lass", TokenType::CLASS);
        case 'e': return CheckKeyword(1, 3, "lse", TokenType::ELSE);
        case 'f':
            if (_current - _start > 1) {
                switch (*(_start + 1)) {
                    case 'a': return CheckKeyword(2, 3, "lse", TokenType::FALSE);
                    case 'o': return CheckKeyword(2, 1, "r", TokenType::FOR);
                    case 'u': return CheckKeyword(2, 1, "n", TokenType::FUN);
                }
            }
            break;
        case 'i': return CheckKeyword(1, 1, "f", TokenType::IF);
        case 'n': return CheckKeyword(1, 2, "il", TokenType::NIL);
        case 'o': return CheckKeyword(1, 1, "r", TokenType::OR);
        case 'p': return CheckKeyword(1, 4, "rint", TokenType::PRINT);
        case 'r': return CheckKeyword(1, 5, "eturn", TokenType::RETURN);
        case 's': return CheckKeyword(1, 4, "uper", TokenType::SUPER);
        case 't':
            if (_current - _start > 1) {
                switch (*(_start + 1)) {
                    case 'h': return CheckKeyword(2, 2, "is", TokenType::THIS);
                    case 'r': return CheckKeyword(2, 2, "ue", TokenType::TRUE);
                }
            }
        case 'v': return CheckKeyword(1, 2, "ar", TokenType::VAR);
        case 'w': return CheckKeyword(1, 4, "hile", TokenType::WHILE);
    }

    return MakeToken(TokenType::IDENTIFIER);
}

// Helper method to build a token with `type`.
auto Scanner::MakeToken(TokenType type) -> Token {
    return { type, _start, static_cast<int>(_current - _start), _line };
}

// Helper method to build a token with error message.
auto Scanner::MakeErrorToken(std::string_view message) -> Token {
    return { TokenType::ERROR, message.begin(), static_cast<int>(message.length()), _line };
}
