#ifndef _NUMBER_GENERATOR_STRONK_H
#define _NUMBER_GENERATOR_STRONK_H

#include <string>

class NumberGenerator {
private:
    int val_;
public:
    auto GenerateNumber() -> int;
    auto GenerateTemp() -> std::string;
};

#endif // _NUMBER_GENERATOR_STRONK_H