#ifndef _STRONK_SCANNER_H
#define _STRONK_SCANNER_H

#include <string>
#include <sstream>
#include <unordered_map>
#include "common.h"
#include "token.h"

// The state of the scanner:
//  - Normal implies the scanner is currently scanning code.
//  - String implies the scanner is scanning a potentially formatted string.
enum class ScannerState {
    NORMAL,
    STRING
};

struct ScannerMode {
    ScannerState state = ScannerState::NORMAL;
    int str_depth = 0;
};

// Lexical analysis over the source to allow for
// compiling.
class Scanner {
private:
    std::ostringstream buffer;

    std::string_view _source;
    std::string_view::iterator _start;
    std::string_view::iterator _current;
    ScannerMode _mode;
    int _line = 1;
    
    auto MakeToken(TokenType type) -> std::shared_ptr<Token>;
    template <class T> auto MakeToken(TokenType type, T value) -> std::shared_ptr<ValueToken<T>>;
    auto MakeTypeToken(PrimitiveType type, int width) -> std::shared_ptr<TypeToken>;
    auto MakeErrorToken(std::string message) -> std::shared_ptr<ValueToken<std::string>>;

    auto MatchChar(char to_match) -> bool;
    void SkipWhitespace();
    auto ScanString() -> std::shared_ptr<Token>;
    auto ScanNumber() -> std::shared_ptr<Token>;
    auto CheckKeyword(int start, int length, std::string_view rest, TokenType type) -> std::shared_ptr<Token>;
    auto CheckTypeKeyword(int start, int length, std::string_view rest, PrimitiveType type, int width) -> std::shared_ptr<Token>;
    auto ScanIdentifier() -> std::shared_ptr<Token>;
    void PushError(std::string message);
public:
    Scanner() = default;
    void LoadSource(std::string_view source);
    auto ScanNextToken() -> std::shared_ptr<Token>;
};

#endif // _STRONK_SCANNER_H