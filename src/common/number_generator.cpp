#include "common/number_generator.h"

auto NumberGenerator::GenerateNumber() -> int {
    return ++val_;
};

auto NumberGenerator::GenerateTemp() -> std::string {
    return "__stronk_temp" + std::to_string(++val_);
}