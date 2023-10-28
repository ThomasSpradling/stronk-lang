#include "scanner.h"

Scanner::Scanner(const std::string_view source) : _start(source.begin()), _current(source.begin()), _line(1), _source(source) {}

auto Scanner::ScanToken() -> Token {
    _start = _current;

    if (_current == _source.end()) {
        return { TokenType::TOKEN_EOF };
    }

    return MakeErrorToken("Unexpected character.");
}

auto Scanner::MakeToken(TokenType type) -> Token {
    return { type, _start, static_cast<int>(_current - _start), _line };
}

auto Scanner::MakeErrorToken(std::string_view message) -> Token {
    return { TokenType::ERROR, message.begin(), static_cast<int>(message.length()), _line };
}