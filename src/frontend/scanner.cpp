#include <iostream>
#include <sstream>
#include "frontend/scanner.h"

namespace stronk {

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
    source_ = source;

    start_ = source_.begin();
    current_ = source_.begin();
}

// Scans next token found in buffer.
auto Scanner::ScanNextToken() -> std::shared_ptr<Token> {
    // String mode should leave whitespace alone.
    if (mode_.state_ != ScannerState::STRING) {
        SkipWhitespace();
    }
    start_ = current_;

    if (current_ == source_.end()) {
        if (mode_.str_depth_ != 0) {
            mode_.str_depth_ = 0;
            return MakeErrorToken("Unterminated string.");
        }
        return MakeToken(TokenType::TOKEN_EOF);
    }

    // String mode activates while scanning within
    // a string.
    if (mode_.state_ == ScannerState::STRING) {
        return ScanString();
    }

    // Get next character and move forward.
    char c = *current_++;

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
            if (mode_.str_depth_ > 0) {
                mode_.state_ = ScannerState::STRING;
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
            mode_.str_depth_++;
            mode_.state_ = ScannerState::STRING;
            return MakeToken(TokenType::QUOTE);
    }

    return MakeErrorToken("Unexpected character.");
}


// Utility mathod to determine whether the current character
// matches the passed in character.
auto Scanner::MatchChar(char to_match) -> bool {
    if (current_ == source_.end() || *current_ != to_match) {
        return false;
    }
    current_++;
    return true;
}

// Utility method to skip all whitespace and comments at
// current character.
void Scanner::SkipWhitespace() {
    for (;;) {
        switch (*current_) {
            case ' ':
            case '\r':
            case '\t':
                current_++;
                break;
            case '\n':
                line_++;
                current_++;
                break;
            case '/':
                if (*(current_ + 1) == '/') {
                    // Comment goes until end of the line.
                    while (*current_ != '\n' && current_ != source_.end()) {
                        current_++;
                    }
                } else if (*(current_ + 1) == '*') {
                    // Multiline comment
                    current_ += 1;
                    int depth = 1; // Keep track of depth of nested comment
                    while (current_ != source_.end()) {
                        if (depth == 0) {
                            current_++;
                            break;
                        }
                        if (*current_ == '\n') {
                            line_++;
                        }
                        current_++;
                        if (*current_ == '/' && *(current_ + 1) == '*') {
                            current_++;
                            depth++;
                        }
                        if (*current_ == '*' && *(current_ + 1) == '/') {
                            current_++;
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
        if (current_ == source_.end()) {
            return MakeErrorToken("Unterminated string.");
        }

        char c = *current_++;

        switch (c) {
            case '\\':
                c = *current_++;
                switch (c) {
                    case '\'': oss << '\''; break;
                    case '\"': oss << '\"'; break;
                    case '\\': oss << "\\"; break;
                    case 'n': oss << "\n"; break;
                    case 'r': oss << "\r"; break;
                    case 't': oss << "\t"; break;
                    case 'b': oss << "\b"; break;
                    case 'f': oss << "\f"; break;
                    case '0': oss << "\0"; break;
                    default:
                        oss << c;
                }
                break;
            case '"':
                mode_.state_ = ScannerState::NORMAL;
                mode_.str_depth_--;
                return MakeToken(TokenType::QUOTE);
            case '\n':
                line_++;
                continue;
            case '$':
                if (MatchChar('{')) {
                    mode_.state_ = ScannerState::NORMAL;
                    return MakeToken(TokenType::DOLLAR_BRACE);
                }
                break;
            default:
                break;
        }

        if (*(current_ - 2) != '\\') {
            oss << *(current_ - 1);
        }
        // If the next characters us to cut string.
        if (current_ == source_.end() || *current_ == '"') {
            std::string value = oss.str();
            return MakeToken<std::string>(TokenType::TEXT, value);
        }
        if (current_ + 1 == source_.end() || (*current_ == '$' && *(current_ + 1) == '{')) {
            std::string value = oss.str();
            return MakeToken<std::string>(TokenType::TEXT, value);
        }
    }
}

// Scans through numbers.
auto Scanner::ScanNumber() -> std::shared_ptr<Token> {
    int value = 0;
    current_--;

    if (isdigit(*current_) == 0) {
        current_++;
        return MakeErrorToken("Expected a digit.");
    }

    while (isdigit(*current_) != 0) {
        value = 10 * value + *current_ - '0';
        current_++;
    }

    if (*current_ != '.') {
        return MakeToken<int>(TokenType::INT, value);
    }
    if (isdigit(*(current_ + 1)) != 0) {
        current_++;
        float float_value = value;
        float d = 10;

        while (isdigit(*current_) != 0) {
            float_value += (*current_ - '0') / d;
            d *= 10;
            current_++;
        }

        return MakeToken<float>(TokenType::REAL, float_value);
    }
    
    return MakeErrorToken("Expected a digit after decimal in literal.");
}

// Helper function for scanning identifier tokens that are keywords.
auto Scanner::CheckKeyword(int start, int length, std::string_view rest, TokenType type) -> std::shared_ptr<Token> {
    std::string_view::iterator start_iter = start_ + start;
    std::string_view view(start_iter, length);
    if (current_ - start_ == start + length && view == rest) {
        return MakeToken(type);
    }

    return MakeToken(TokenType::IDENTIFIER);
}

// Helper function for scanning identifier tokens that are type keywords.
auto Scanner::CheckTypeKeyword(int start, int length, std::string_view rest, PrimitiveType type, int width) -> std::shared_ptr<Token> {
    std::string_view::iterator start_iter = start_ + start;
    std::string_view view(start_iter, length);
    if (current_ - start_ == start + length && view == rest) {
        return MakeTypeToken(type, width);
    }

    return MakeToken(TokenType::IDENTIFIER);
}

// Scans identifier, which begin with letter (or underscore) and
// with all other characters being letters, underscores, or numbers.
auto Scanner::ScanIdentifier() -> std::shared_ptr<Token> {
    std::ostringstream oss;
    current_--;
    while (isalpha(*current_) != 0) {
        oss << *current_;
        current_++;
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
    return std::make_unique<Token>(type, 0, line_);
}

// Helper method to build a value token: One that contains additional value
// information.
template <class T>
auto Scanner::MakeToken(TokenType type, T value) -> std::shared_ptr<ValueToken<T>> {
    return std::make_unique<ValueToken<T>>(type, 0, line_, value);
}

// Helper method to build a type token, One with two pieces of information -- name and width.
auto Scanner::MakeTypeToken(PrimitiveType type, int width) -> std::shared_ptr<TypeToken> {
    return std::make_unique<TypeToken>(TokenType::PRIMITIVE, 0, line_, type, width);
}

// Helper method to build a token with error message.
auto Scanner::MakeErrorToken(std::string message) -> std::shared_ptr<ValueToken<std::string>> {
    return std::make_unique<ValueToken<std::string>>(TokenType::ERROR, 0, line_, std::move(message));
}

} // namespace "stronk"