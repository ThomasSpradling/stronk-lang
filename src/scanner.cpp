#include <iostream>
#include <sstream>
#include "scanner.h"

/***** Nonmember Functions ***/

// Determines if character a letter or underscore.
auto IsAlpha(char c) -> bool {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; 
}


/***** Member Methods ********/

std::unordered_map<std::string, TokenType> reserved_keywords {
    { "and", TokenType::AND },
    { "class", TokenType::CLASS },
    { "else", TokenType::ELSE },
    { "false", TokenType::FALSE },
    { "for", TokenType::FOR },
    { "func", TokenType::FUN },
    { "if", TokenType::IF },
    { "nil", TokenType::NIL },
    { "or", TokenType::OR },
    { "print", TokenType::PRINT },
    { "return", TokenType::RETURN },
    { "super", TokenType::SUPER },
    { "this", TokenType::THIS },
    { "true", TokenType::TRUE },
    { "var", TokenType::VAR },
    { "while", TokenType::WHILE },
};

std::unordered_map<std::string, std::pair<PrimitiveType, int>> reserved_typenames {
    { "int", { PrimitiveType::INT, _STRONK_INT_WIDTH } },
    { "real", { PrimitiveType::REAL, _STRONK_FLOAT_WIDTH } },
    { "char", { PrimitiveType::CHAR, 1 } },
    { "bool", { PrimitiveType::BOOL, 1 } }
};

// Loads source into buffer.
void Scanner::LoadSource(std::string_view source) {
    _source = source;

    _start = _source.begin();
    _current = _source.begin();
}

// Scans next token found in buffer.
auto Scanner::ScanNextToken() -> std::shared_ptr<Token> {
    // String mode should leave whitespace alone.
    if (_mode.state != ScannerState::STRING) {
        SkipWhitespace();
    }
    _start = _current;

    if (_current == _source.end()) {
        if (_mode.str_depth != 0) {
            _mode.str_depth = 0;
            return MakeErrorToken("Unterminated string.");
        }
        return MakeToken(TokenType::TOKEN_EOF);
    }

    // String mode activates while scanning within
    // a string.
    if (_mode.state == ScannerState::STRING) {
        return ScanString();
    }

    // Get next character and move forward.
    char c = *_current++;

    if (IsAlpha(c)) {
        return ScanIdentifier();
    }
    if (isdigit(c) != 0) {
        return ScanNumber();
    }

    switch (c) {
        case '(': return MakeToken(TokenType::LEFT_PAREN);
        case ')': return MakeToken(TokenType::RIGHT_PAREN);
        case '[': return MakeToken(TokenType::LEFT_BRACKET);
        case ']': return MakeToken(TokenType::RIGHT_BRACKET);
        case '{': return MakeToken(TokenType::LEFT_BRACE);
        case '}':
            if (_mode.str_depth > 0) {
                _mode.state = ScannerState::STRING;
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
            _mode.str_depth++;
            _mode.state = ScannerState::STRING;
            return MakeToken(TokenType::QUOTE);
    }

    return MakeErrorToken("Unexpected character.");
}


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
                } else if (*(_current + 1) == '*') {
                    // Multiline comment
                    _current += 1;
                    int depth = 1; // Keep track of depth of nested comment
                    while (_current != _source.end()) {
                        if (depth == 0) {
                            _current++;
                            break;
                        }
                        if (*_current == '\n') {
                            _line++;
                        }
                        _current++;
                        if (*_current == '/' && *(_current + 1) == '*') {
                            _current++;
                            depth++;
                        }
                        if (*_current == '*' && *(_current + 1) == '/') {
                            _current++;
                            depth--;
                        }
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

// Assumes in string mode. Gets next token found in string.
auto Scanner::ScanString() -> std::shared_ptr<Token> {
    std::ostringstream oss;
    for (;;) {
        if (_current == _source.end()) {
            return MakeErrorToken("Unterminated string.");
        }

        char c = *_current++;

        switch (c) {
            case '\\':
                // Escaped characters are their own tokens.
                _current++;
                return MakeToken<char>(TokenType::ESCAPED, *_current);
            case '"':
                _mode.state = ScannerState::NORMAL;
                _mode.str_depth--;
                return MakeToken(TokenType::QUOTE);
            case '$':
                if (MatchChar('{')) {
                    _mode.state = ScannerState::NORMAL;
                    return MakeToken(TokenType::DOLLAR_BRACE);
                }
            case '\n':
                _line++;
                break;
            default:
                oss << *(_current - 1);
                // If the next characters us to cut string.
                if (_current == _source.end() || *_current == '"' || *_current  == '\\') {
                    std::string value = oss.str();
                    return MakeToken<std::string>(TokenType::TEXT, value);
                }
                if (_current + 1 == _source.end() || *_current == '$' && *(_current + 1) == '{') {
                    std::string value = oss.str();
                    return MakeToken<std::string>(TokenType::TEXT, value);
                }
        }
    }
}

// Scans through numbers.
auto Scanner::ScanNumber() -> std::shared_ptr<Token> {
    int value = 0;
    _current--;

    if (isdigit(*_current) == 0) {
        _current++;
        return MakeErrorToken("Expected a digit.");
    }

    while (isdigit(*_current) != 0) {
        value = 10 * value + *_current - '0';
        _current++;
    }

    if (*_current != '.') {
        return MakeToken<int>(TokenType::INT, value);
    }

    if (isdigit(*(_current + 1)) != 0) {
        _current++;

        float float_value = value;
        float d = 10;

        while (isdigit(*_current) != 0) {
            _current++;
            float_value += (*_current - '0') / d;
            d *= 10;
        }

        return MakeToken<float>(TokenType::REAL, float_value);
    }
    
    return MakeErrorToken("Expected a digit after decimal in literal.");
}

// Helper function for scanning identifier tokens that are keywords.
auto Scanner::CheckKeyword(int start, int length, std::string_view rest, TokenType type) -> std::shared_ptr<Token> {
    std::string_view::iterator start_iter = _start + start;
    std::string_view view(start_iter, length);
    if (_current - _start == start + length && view == rest) {
        return MakeToken(type);
    }

    return MakeToken(TokenType::IDENTIFIER);
}

// Helper function for scanning identifier tokens that are type keywords.
auto Scanner::CheckTypeKeyword(int start, int length, std::string_view rest, PrimitiveType type, int width) -> std::shared_ptr<Token> {
    std::string_view::iterator start_iter = _start + start;
    std::string_view view(start_iter, length);
    if (_current - _start == start + length && view == rest) {
        return MakeTypeToken(type, width);
    }

    return MakeToken(TokenType::IDENTIFIER);
}

// Scans identifier, which begin with letter (or underscore) and
// with all other characters being letters, underscores, or numbers.
auto Scanner::ScanIdentifier() -> std::shared_ptr<Token> {
    std::ostringstream oss;
    _current--;
    while (isalpha(*_current) != 0) {
        oss << *_current;
        _current++;
    }
    std::string id = oss.str();

    if (reserved_keywords.find(id) != reserved_keywords.end()) {
        return MakeToken(reserved_keywords[id]);
    }
    
    if (reserved_typenames.find(id) != reserved_typenames.end()) {
        auto [type, width] = reserved_typenames[id];
        return MakeTypeToken(type, width);
    }

    return MakeToken<std::string>(TokenType::IDENTIFIER, id);
}

// Helper method to build a token with `type`.
auto Scanner::MakeToken(TokenType type) -> std::shared_ptr<Token> {
    return std::make_unique<Token>(type, 0, _line);
}

// Helper method to build a value token: One that contains additional value
// information.
template <class T>
auto Scanner::MakeToken(TokenType type, T value) -> std::shared_ptr<ValueToken<T>> {
    return std::make_unique<ValueToken<T>>(type, 0, _line, value);
}

// Helper method to build a type token, One with two pieces of information -- name and width.
auto Scanner::MakeTypeToken(PrimitiveType type, int width) -> std::shared_ptr<TypeToken> {
    return std::make_unique<TypeToken>(TokenType::PRIMITIVE, 0, _line, type, width);
}

// Helper method to build a token with error message.
auto Scanner::MakeErrorToken(std::string message) -> std::shared_ptr<ValueToken<std::string>> {
    return std::make_unique<ValueToken<std::string>>(TokenType::ERROR, 0, _line, std::move(message));
}