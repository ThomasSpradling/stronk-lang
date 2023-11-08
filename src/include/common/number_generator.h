#ifndef _NUMBER_GENERATOR_STRONK_H
#define _NUMBER_GENERATOR_STRONK_H

#include <string>
#include "common.h"

namespace stronk {

class NumberGenerator {
private:
    int val_ = 0;
public:
    auto GenerateNumber() -> int;
    auto GenerateTemp() -> std::string;
};

} // namespace "stronk"

#endif // _NUMBER_GENERATOR_STRONK_H