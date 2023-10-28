#ifndef _STRONK_SCANNER_H
#define _STRONK_SCANNER_H

#include <string>

// Lexical analysis over the source to allow for
// compiling.
class Scanner {
private:
    std::string::iterator _start;
    std::string::iterator _current;
    int _line;
public:
    Scanner(std::string source);
};

#endif // _STRONK_SCANNER_H