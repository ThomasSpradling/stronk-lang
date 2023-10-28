#include "scanner.h"

Scanner::Scanner(const std::string_view source) : _start(source.begin()), _current(source.begin()), _line(1) {}
