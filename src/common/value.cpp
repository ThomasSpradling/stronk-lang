#include <iostream>
#include <iomanip>
#include "common/value.h"

// Prints a single value with precision 6.
void PrintValue(Value* val) {
    if (auto bool_val = dynamic_cast<BoolValue*>(val)) {
        std::cout << "BoolValue: " << bool_val->val_ << std::endl;
    } else if (auto real_val = dynamic_cast<RealValue*>(val)) {
        std::cout << "RealValue: " << real_val->val_ << std::endl;
    } else if (auto int_val = dynamic_cast<IntValue*>(val)) {
        std::cout << "IntValue: " << int_val->val_ << std::endl;
    } else if (auto char_val = dynamic_cast<CharValue*>(val)) {
        std::cout << "CharValue: " << char_val->val_ << std::endl;
    } else if (auto nul_val = dynamic_cast<NilValue*>(val)) {
        std::cout << "NilValue" << std::endl;
    } else {
        std::cout << "Unknown Value Type" << std::endl;
    }
}
