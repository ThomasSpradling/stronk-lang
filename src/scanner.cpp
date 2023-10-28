#include "scanner.h"

Scanner::Scanner(std::string source) : _start(source.begin()), _current(source.begin()), _line(1) {}
