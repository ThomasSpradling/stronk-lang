#include "common/number_generator.h"

auto NumberGenerator::GenerateNumber() -> int {
    return val_++;
};

auto NumberGenerator::GenerateTemp() -> std::string {
    return TEMP_VAR_PREFIX + std::to_string(++val_);
}