#ifndef _STRONK_SCANNER_H
#define _STRONK_SCANNER_H

#include <string>

// Lexical analysis over the source to allow for
// compiling.
class Scanner {
private:
    std::string_view::iterator _start;
    std::string_view::iterator _current;
    int _line;
public:
    Scanner(const std::string_view source);
};

#endif // _STRONK_SCANNER_H